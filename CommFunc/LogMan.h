#ifndef LOGMAN_H
#define LOGMAN_H

#include <QtDebug>
#include "CommFunc.h"

class FrameLogInf
{
public:
    enum DIR{RECEIVE, SEND};

    virtual void FrameLog(FrameLogInf::DIR dir, const void *data, int len) = 0;
    virtual void Log(const QString &log) = 0;
    virtual void ShowStatus(const QString &status) = 0;
};
extern "C"
{
COMMFUNCSHARED_EXPORT void MyMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
}
#endif // LOGMAN_H
