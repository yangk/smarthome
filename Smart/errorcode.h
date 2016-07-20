#ifndef ERRORCODE_H
#define ERRORCODE_H

//---------------------------------------------------------------------------------------
#include <stdint.h>
#include <QString>
//---------------------------------------------------------------------------------------
enum SMART_ERROR
{
    SMART_ENO,
    SMART_ESERIAL_NOOPEN,
    SMART_ETMOUT,
    SMART_ENODATA,
    SMART_EBUSY,
    SMART_USR,
};
//---------------------------------------------------------------------------------------
const QString &SmartError(uint32_t err);
//---------------------------------------------------------------------------------------
#endif // ERRORCODE_H

