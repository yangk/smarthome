#ifndef SMARTFRAME_H
#define SMARTFRAME_H

#include <QString>
#include "../CommFunc/Commfunc.h"

//---------------------------------------------------------------------------------------
#define STC             0x7e
#define AID_LEN         0x04
#define DID_LEN         0x02
#define SID_LEN         0x02
#define PW_LEN          0x02
//---------------------------------------------------------------------------------------
#define CMD_SET_AID     0x01
#define CMD_GET_AID     0x03
#define CMD_ACK_AID     0x13
#define CMD_DEL_AID     0x04
#define CMD_REQ_AID     0x05
#define CMD_GET_SID     0x06
#define CMD_ACK_SID     0x16
#define CMD_GET_EID     0x07
#define CMD_ACK_EID     0x17
#define CMD_SET_BPS     0x08
#define CMD_SET_REG     0x09
#define CMD_UNLINK      0x0A
#define CMD_REGINFOR    0x0B
#define CMD_SET_PANID   0x0C
#define CMD_GET_GWAID   0x0D
#define CMD_GET_VER     0x0E
#define CMD_ACK_VER     0x1E
#define CMD_GET_PANID   0x0F
#define CMD_ACK_PANID   0x1F

#define CMD_ACK         0x00
#define CMD_NAK         0xFF

#define CMD_SET         0x07
#define CMD_READ        0x02
#define CMD_UPDATE      0x05
#define CMD_UPDATE_PLC  0x06
#define CMD_RELI_REPORT    0x01
#define CMD_NRELI_REPORT   0x00

//---------------------------------------------------------------------------------------
#define BROADCAST_ADDR  0xFFFFFFFF
//---------------------------------------------------------------------------------------
#define DID_REG     0x0603
#define DID_EXTVER  0x0004
#define DID_INTVER  0x0606
#define DID_GROUPNO 0x0607
#define DID_KEY     0x0608
//---------------------------------------------------------------------------------------
#pragma pack(1)
struct SmartFrame
{
    uint8_t stc;
    uint8_t said[AID_LEN];
    uint8_t taid[AID_LEN];
    uint8_t seq;
    uint8_t len;
    uint8_t data[0];
};
#define SMART_FRAME_HEAD ((int)sizeof(struct SmartFrame))

struct App
{
    uint8_t cmd;
    uint8_t data[0];
};
struct GroupFrame
{
    uint8_t len : 6;
    uint8_t type: 2;
    uint8_t data[0];
};

struct Body
{
    uint8_t did[2];
    uint8_t ctrl;
    uint8_t data[0];
};
#define FBD_FRAME_HEAD  ((int)sizeof(struct Body))

struct RegData
{
    uint8_t aid[AID_LEN];
    uint8_t panid[SID_LEN];
    uint8_t pw[PW_LEN];
    uint8_t gid[AID_LEN];
    uint8_t sid[SID_LEN];
};
#pragma pack()

#define frame_len(frame) (SMART_FRAME_HEAD + frame->len + 1)

static inline bool is_local_frame(struct SmartFrame *pframe)
{
    return is_all_xx(pframe->said, 0x00, AID_LEN) && is_all_xx(pframe->taid, 0x00, AID_LEN);
}
static inline bool is_remote_frame(struct SmartFrame *pframe)
{
    return !is_local_frame(pframe);
}
static inline bool is_reply_frame(struct SmartFrame *pframe)
{
    return tst_bit(pframe->seq, 7);
}
static inline bool is_broad_frame(struct SmartFrame *pframe)
{
    return is_all_xx(pframe->taid, 0xFF, sizeof(pframe->taid));
}
static inline int get_local_ack_cmd(struct SmartFrame *pframe)
{
    return pframe->data[0];
}
//---------------------------------------------------------------------------------------
struct SmartFrame *get_smart_frame(const uint8_t *in, size_t len);
//---------------------------------------------------------------------------------------
int CodeFrame(uint32_t src, uint32_t dest, int cmd,
               int gtype, const uint8_t *gdata, int glen,
               const uint8_t *did, const void *data, int len,
               uint8_t *out, size_t maxlen);
//---------------------------------------------------------------------------------------
int CodeUnicastFrame(uint32_t src, uint32_t dest,
                   int cmd, uint16_t did, const void *data, int len,
                   void *out, size_t maxlen);
//---------------------------------------------------------------------------------------
int CodeUpdateFrame(uint32_t src, uint32_t dest,
                    uint8_t cmd, const void *data, int len,
                    uint8_t *out, size_t maxlen);
//---------------------------------------------------------------------------------------
const struct Body *GetBody(const struct SmartFrame *pframe, uint32_t did);
int local_ack_opt(void *arg);
int get_eid(void *out, int maxlen);
int code_local_frame(int cmd, const uint8_t *in, int len, void *out, int maxlen);
//---------------------------------------------------------------------------------------

QString GetCmdDesp(bool local, int cmd);
//---------------------------------------------------------------------------------------
#endif // SMARTFRAME_H
