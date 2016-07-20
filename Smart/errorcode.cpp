#include "errorcode.h"
#include "../CommFunc/Commfunc.h"

//---------------------------------------------------------------------------------------
const QString noDefErr("未定义错误");
//---------------------------------------------------------------------------------------
struct ErrorInfo
{
    uint32_t error;
    QString info;
};
static struct ErrorInfo infos[] =
{
    {SMART_ESERIAL_NOOPEN, "串口未打开"},
    {SMART_ETMOUT, "超时"},
    {SMART_ENODATA,"无数据"},
    {SMART_EBUSY,  "设备忙"},
    {SMART_USR,    "您操作错误"},
};
//---------------------------------------------------------------------------------------
const QString &SmartError(uint32_t err)
{
    for (size_t i = 0; i < array_size(infos); ++i)
    {
        if (infos[i].error == err)
            return infos[i].info;
    }
    return noDefErr;
}
//---------------------------------------------------------------------------------------
