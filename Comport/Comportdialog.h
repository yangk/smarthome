#ifndef COMPORTDIALOG_H
#define COMPORTDIALOG_H

#include <QtCore/qglobal.h>
#include <QDialog>
#include <QList>

#include <stdint.h>

#if defined(COMPORT_LIBRARY)
#  define COMPORTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMPORTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define SETTING_FILE "./System.ini"

namespace Ui {
class ComportDialog;
}

class ComInf
{
public:
    virtual int OnSerialData(const void *data, int len)
    {Q_UNUSED(data);return len;}
    virtual int OnTcpData(const void *data, int len)
    {Q_UNUSED(data);return len;}
    virtual int OnUdpData(const void *data, int len)
    {Q_UNUSED(data);return len;}
};

class QTimer;
class QSettings;
class QextSerialPort;
class QextPortInfo;
class QextSerialEnumerator;
class QAbstractSocket;
class QTcpSocket;
class QUdpSocket;
class FrameLogInf;
class COMPORTSHARED_EXPORT ComportDialog : public QDialog
{
    Q_OBJECT

public:
    enum ComChn
    {
        ComChnTCP, ComChnUDP, ComChnSerial,
    };

    explicit ComportDialog(FrameLogInf *frameLog, QWidget *parent = 0);
    ~ComportDialog();

    int read(void *out, int maxlen);
    void write(ComChn chn, const void *writeData, int len);
    bool IsCommOpen(void);

    void AddSubscriber(ComInf *sub);
    void DelSubscriber(ComInf *sub);

    int SetBaud(int baud);

signals:
    void sig_serial_open(bool, QString&);

private slots:
    void on_pushButton_action_clicked();
    void OnTimerOut();
    void OnSerialReadyRead();
    void onPortAddedOrRemoved(void);

    void OnTcpConnected();
    void OnTcpDisconnected();
    void OnTcpError();
    void OnTcpReadyRead();
    void OnUdpReadyRead();

    void on_checkBoxAutoLocalPort_toggled(bool checked);
    void on_pushButtonConnect_clicked();

private:
    FrameLogInf *frameLog;

    Ui::ComportDialog *ui;
    QSettings *setting;

    QextSerialPort *port;
    QextSerialEnumerator *enumerator;
    QTimer *timer;
    QByteArray cacheComport;

    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    bool udpSocketConnected;

    QList<ComInf*> subs;

    void UpdateNetUi();
    int DoBindPort(QAbstractSocket *socket);
    void DoTcpConnect();
    void DoUdpConnect();
    void SetupComport();
    void SetupNetWork();
    void LoadSerialSetting(QSettings *setting);
    void SaveSerialSetting(QSettings *setting);
    QString GetSerialPortSetting(void);
};

#endif // COMPORTDIALOG_H
