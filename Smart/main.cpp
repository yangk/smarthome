#include <QtCore>
#include <QtWidgets>
#include "LogMan.h"
#include "Database/ddm.h"
#include "mainwindow.h"

//---------------------------------------------------------------------------------------
#define Ver 1.10
#define AppName "智能终端开发测试软件"
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(MyMessageOutput);
    a.setApplicationName(AppName);
    a.setApplicationVersion(QString::number(Ver, 'f', 2));

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QFile qss(":/File/mystyle.qss");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    DDMSetup();

    MainWindow w;
    w.show();

    return a.exec();
}
//---------------------------------------------------------------------------------------
