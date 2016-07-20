#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include "LogMan.h"

#define LOG_FILE  "SmartHome.log"

//---------------------------------------------------------------------------------------
void MyMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QString txt;
    switch (type)
    {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QFile outFile(LOG_FILE);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}
//---------------------------------------------------------------------------------------
