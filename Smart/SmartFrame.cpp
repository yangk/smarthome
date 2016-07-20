#include <stdlib.h>
#include <string.h>
#include "SmartFrame.h"
#include "Commfunc.h"
//---------------------------------------------------------------------------------------
struct SmartFrame *get_smart_frame(const uint8_t *in, size_t len)
{
    size_t i = 0;

 start_lbl:
    while (i < len)
    {
        if (STC == in[i])
            break;
        i++;
    }
    if (len - i < SMART_FRAME_HEAD)
        return NULL;
    struct SmartFrame *pframe = (struct SmartFrame *)&in[i];
    int dlen = pframe->len;

    if (i + SMART_FRAME_HEAD + dlen + 1 > len)
    {
        i++;
        goto start_lbl;
    }

    if (pframe->data[dlen] != checksum(pframe, dlen + SMART_FRAME_HEAD))
    {
        i++;
        goto start_lbl;
    }
    pframe = (struct SmartFrame *)&in[i];
    return pframe;
}

//---------------------------------------------------------------------------------------
int CodeFrame(uint32_t src, uint32_t dest, int cmd, int gtype, const uint8_t *gdata, int glen,
               const uint8_t *did, const void *data, int len, uint8_t *out, size_t maxlen)
{
    static uint32_t seq = 0;

    /* Code Frame Section */
    if (sizeof(struct SmartFrame) > maxlen) return -1;
    struct SmartFrame *pframe = (struct SmartFrame *)out;
    pframe->stc = STC;
    put_le_val(src, pframe->said, sizeof(pframe->said));
    put_le_val(dest, pframe->taid, sizeof(pframe->taid));
    pframe->seq = seq++ & 0x7F;

    /* Code App Section */
    if (sizeof(struct SmartFrame) + sizeof(struct App) > maxlen) return -1;
    struct App *app = (struct App *)pframe->data;
    app->cmd = cmd;
    int idx = 0;

    /* Code Group Section */
    if (gtype >= 0)
    {
        struct GroupFrame *g = (struct GroupFrame *)&app->data[idx];
        g->type = gtype;
        g->len = glen;
        memcpy(g->data, gdata, glen);
        idx += sizeof(struct GroupFrame) + glen;
    }

    if (did)
    {
        /* Code Body Section */
        struct Body *body = (struct Body *)&app->data[idx];
        memcpy(body->did, did, sizeof(body->did));
        memcpy(body->data, data, len);
        body->ctrl = len;
        idx += sizeof(struct Body) + len;
    }
    else
    {
        memcpy(app->data, data, len);
        idx += len;
    }

    idx += sizeof(struct App);
    pframe->len = idx;
    pframe->data[idx] = checksum(pframe, sizeof(SmartFrame) + idx);

    return sizeof(SmartFrame) + idx + 1;
}
//---------------------------------------------------------------------------------------
int CodeUnicastFrame(uint32_t src, uint32_t dest,
                   int cmd, uint16_t did, const void *data, int len,
                   void *out, size_t maxlen)
{
    uint8_t tmp[2];
    put_le_val(did, tmp, sizeof(tmp));
    return CodeFrame(src, dest, cmd, -1, NULL, 0, tmp, data, len, (uint8_t*)out, maxlen);
}
//---------------------------------------------------------------------------------------
int CodeUpdateFrame(uint32_t src, uint32_t dest,
                    uint8_t cmd, const void *data, int len,
                    uint8_t *out, size_t maxlen)
{
    return CodeFrame(src, dest, cmd, -1, NULL, 0, NULL, data, len, out, maxlen);
}

//---------------------------------------------------------------------------------------
int code_local_frame(int cmd, const uint8_t *in, int len, void *out, int maxlen)
{
    if (maxlen < SMART_FRAME_HEAD + len + 1 + 1) return -1;

    struct SmartFrame *pframe = (struct SmartFrame *)out;
    pframe->stc = STC;
    memset(pframe->said, 0x00, AID_LEN);
    memset(pframe->taid, 0x00, AID_LEN);
    pframe->seq = 0;

    pframe->len = len + 1;
    pframe->data[0] = cmd;
    memcpy(&pframe->data[1], in, len);
    pframe->data[pframe->len] = checksum(pframe, SMART_FRAME_HEAD + pframe->len);
    return SMART_FRAME_HEAD + pframe->len + 1;
}
//---------------------------------------------------------------------------------------
int local_ack_opt(void *arg)
{
    struct SmartFrame *pframe = (struct SmartFrame *)arg;

    if (!is_all_xx(pframe->said, 0x00, AID_LEN) || !is_all_xx(pframe->taid, 0x00, AID_LEN))
        return -1;

    uint8_t cmd = pframe->data[0];
    if (cmd == CMD_ACK || cmd == CMD_NAK)
        return 0;

    return -1;
}
//---------------------------------------------------------------------------------------
int get_eid(void *out, int maxlen)
{
    return code_local_frame(CMD_GET_EID, NULL, 0, out, maxlen);
}

const struct Body *GetBody(const struct SmartFrame *pframe, uint32_t did)
{
    int len = pframe->len - sizeof(struct App);
    struct App *app = (struct App *)pframe->data;

    if (app->cmd != CMD_READ && app->cmd != CMD_SET) return NULL;

    int dlen;
    for (int inidx = 0; inidx < len; inidx += FBD_FRAME_HEAD + dlen)
    {
        const struct Body *body = (const struct Body *)&app->data[inidx];

        dlen = get_bits(body->ctrl, 0, 6);
        if (len - inidx < FBD_FRAME_HEAD + dlen)
            return NULL;

        if (get_le_val(body->did, sizeof(body->did)) == did)
            return body;
    }
    return NULL;
}
//---------------------------------------------------------------------------------------
struct CmdDesp
{
    int cmd;
    const char *desp;
};
static const struct CmdDesp localCmds[] =
{
    {CMD_ACK, "确认应答"},
    {CMD_NAK, "否认应答"},
    {CMD_SET_AID   , "设置应用层地址"},
    {CMD_GET_AID   , "获取应用层地址"},
    {CMD_ACK_AID   , "回复应用层地址"},
    {CMD_DEL_AID   , "清除应用层地址"},
    {CMD_REQ_AID   , "请求应用层地址"},
    {CMD_GET_SID   , "获取短地址"},
    {CMD_ACK_SID   , "回复短地址"},
    {CMD_GET_EID   , "获取EID"},
    {CMD_ACK_EID   , "回复EID"},
    {CMD_SET_BPS   , "设置波特率"},
    {CMD_SET_REG   , "请求注册"},
    {CMD_UNLINK    , "断开连接"},
    {CMD_REGINFOR  , "设置注册信息"},
    {CMD_SET_PANID , "设置PANID"},
    {CMD_GET_GWAID , "获取网关地址"},
    {CMD_GET_VER   , "获取版本号"},
    {CMD_ACK_VER   , "回复版本号"},
    {CMD_GET_PANID , "获取PANID"},
    {CMD_ACK_PANID , "回复PANID"},
};
static const struct CmdDesp remoteCmds[] =
{
    {CMD_SET , "设置"},
    {CMD_READ, "读取"},
    {CMD_UPDATE,     "应用层升级"},
    {CMD_UPDATE_PLC, "网络层升级"},
};

QString GetCmdDesp(bool local, int cmd)
{
    const struct CmdDesp *ptr = local ? localCmds:remoteCmds;
    int ptrCnt = local ? array_size(localCmds):array_size(remoteCmds);

    for (int i = 0; i < ptrCnt; i++)
    {
        if (ptr[i].cmd == cmd)
            return ptr[i].desp;
    }
    return "";
}
