#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QTimer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include "Comportdialog.h"
#include "ui_Comportdialog.h"
#include "Commfunc.h"
#include "LogMan.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"

//---------------------------------------------------------------------------------------
const int bauds[]    = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
const int databits[] = {5, 6, 7, 8};
const char *stopbits[] = {"1", "1_5", "2"};
const char *parity[] = {"None", "Odd", "Even", "Mark", "Space"};
//---------------------------------------------------------------------------------------
ComportDialog::ComportDialog(FrameLogInf *frameLog, QWidget *parent) :
    QDialog(parent), ui(new Ui::ComportDialog)
{
    ui->setupUi(this);

    udpSocketConnected = false;

    this->frameLog = frameLog;
    setting = new QSettings(SETTING_FILE, QSettings::IniFormat);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(OnTimerOut()));

    SetupComport();
    SetupNetWork();
}

ComportDialog::~ComportDialog()
{
    SaveSerialSetting(setting);
    delete setting;
    delete ui;
}
//---------------------------------------------------------------------------------------
void ComportDialog::SetupComport()
{
    for (size_t i = 0; i < array_size(bauds); i++)
         ui->comboBox_baud->addItem(QString::number(bauds[i]));

    for (size_t i = 0; i < array_size(databits); i++)
         ui->comboBox_databits->addItem(QString::number(databits[i]));

    for (size_t i = 0; i < array_size(parity); i++)
         ui->comboBox_parity->addItem(parity[i]);

    for (size_t i = 0; i < array_size(stopbits); i++)
         ui->comboBox_stopbits->addItem(stopbits[i]);

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();
    port = new QextSerialPort(QextSerialPort::EventDriven, this);

    onPortAddedOrRemoved();

    LoadSerialSetting(setting);

    connect(port, SIGNAL(readyRead()), SLOT(OnSerialReadyRead()));

    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
}

void ComportDialog::SetupNetWork()
{
    QRegExpValidator *v = new QRegExpValidator(this);
    QRegExp rx("((1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})\\.)"
               "{3,3}(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})");
    v->setRegExp(rx);
    ui->lineEditRemoteIP->setValidator(v);

    on_checkBoxAutoLocalPort_toggled(true);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), SLOT(OnTcpConnected()));
    connect(tcpSocket, SIGNAL(disconnected()), SLOT(OnTcpDisconnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),SLOT(OnTcpError()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(OnTcpReadyRead()));

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), SLOT(OnUdpReadyRead()));
}

void ComportDialog::LoadSerialSetting(QSettings *setting)
{
    ui->comboBox_port->setCurrentText(setting->value("Serial/PortName").toString());
    ui->comboBox_baud->setCurrentText(setting->value("Serial/Baud", "9600").toString());
    ui->comboBox_databits->setCurrentText(setting->value("Serial/Databits", "8").toString());
    ui->comboBox_parity->setCurrentText(setting->value("Serial/Parity", "None").toString());
    ui->comboBox_stopbits->setCurrentText(setting->value("Serial/Stopbits", "1").toString());

    ui->lineEditRemoteIP->setText(setting->value("NetWork/RemoteIP", "127.0.0.1").toString());
    ui->spinBoxRemotePort->setValue(setting->value("NetWork/RemotePort", "7000").toInt());
    ui->spinBoxLocalPort->setValue(setting->value("NetWork/LocalPort", "7001").toInt());
    ui->comboBoxNetType->setCurrentText(setting->value("NetWork/Type", "UDP").toString());
    ui->checkBoxAutoLocalPort->setChecked(setting->value("NetWork/AutoLocalPort", "true").toBool());
}
void ComportDialog::SaveSerialSetting(QSettings *setting)
{
    setting->setValue("Serial/PortName", ui->comboBox_port->currentText());
    setting->setValue("Serial/Baud", ui->comboBox_baud->currentText());
    setting->setValue("Serial/Databits", ui->comboBox_databits->currentText());
    setting->setValue("Serial/Parity", ui->comboBox_parity->currentText());
    setting->setValue("Serial/Stopbits", ui->comboBox_stopbits->currentText());

    setting->setValue("NetWork/RemoteIP", ui->lineEditRemoteIP->text());
    setting->setValue("NetWork/RemotePort", ui->spinBoxRemotePort->value());
    setting->setValue("NetWork/LocalPort", ui->spinBoxLocalPort->value());
    setting->setValue("NetWork/Type", ui->comboBoxNetType->currentText());
    setting->setValue("NetWork/AutoLocalPort", ui->checkBoxAutoLocalPort->isChecked());
}
QString ComportDialog::GetSerialPortSetting(void)
{
    QString text;
    text = QString("%1,%2,%3%4%5")
            .arg(ui->comboBox_port->currentText())
            .arg(ui->comboBox_baud->currentText())
            .arg(ui->comboBox_databits->currentText())
            .arg(ui->comboBox_parity->currentText().at(0))
            .arg(ui->comboBox_stopbits->currentText());
    return text;
}

int ComportDialog::SetBaud(int baud)
{
    int baud_pre = ui->comboBox_baud->currentText().toInt();

    ui->comboBox_baud->setCurrentText(QString::number(baud));
    if (port->isOpen())
        port->setBaudRate((BaudRateType)baud);
    QString text = GetSerialPortSetting();
    emit sig_serial_open(port->isOpen(), text);
    return baud_pre;
}

bool ComportDialog::IsCommOpen(void)
{
    return port->isOpen();
}

void ComportDialog::AddSubscriber(ComInf *sub)
{
    subs.append(sub);
}
void ComportDialog::DelSubscriber(ComInf *sub)
{
    subs.removeOne(sub);
}
//---------------------------------------------------------------------------------------
void ComportDialog::onPortAddedOrRemoved(void)
{
    QString current = ui->comboBox_port->currentText();

    ui->comboBox_port->blockSignals(true);
    ui->comboBox_port->clear();

    foreach (const QextPortInfo &info, QextSerialEnumerator::getPorts())
        ui->comboBox_port->addItem(info.portName);

    ui->comboBox_port->setCurrentIndex(ui->comboBox_port->findText(current));

    ui->comboBox_port->blockSignals(false);
}
//---------------------------------------------------------------------------------------
void ComportDialog::on_pushButton_action_clicked()
{
    if (port->isOpen())
    {
        port->close();
    }
    else
    {
        port->setPortName(ui->comboBox_port->currentText());
        port->setBaudRate((BaudRateType)ui->comboBox_baud->currentText().toInt());
        port->setDataBits((DataBitsType)ui->comboBox_databits->currentText().toInt());
        port->setParity((ParityType)ui->comboBox_parity->currentIndex());
        port->setStopBits((StopBitsType)ui->comboBox_stopbits->currentIndex());
        port->setTimeout(ui->spinBox_byteTimeout->value());
        port->setTimeout(1000);
        if (!port->open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this,tr("打开%1失败").arg(port->portName()),tr("端口被占用或其他错误"));
            return;
        }
        port->setTimeout(1000);
    }

    QString text = GetSerialPortSetting();
    emit sig_serial_open(port->isOpen(), text);
    ui->pushButton_action->setText(port->isOpen() ? tr("关闭") : tr("打开"));
    ui->groupBox->setEnabled(!port->isOpen());
    if (port->isOpen()) this->close();
}

void ComportDialog::OnTimerOut()
{
    timer->stop();

    frameLog->FrameLog(FrameLogInf::RECEIVE, cacheComport.data(), cacheComport.length());

    foreach (ComInf *sub, subs)
    {
        sub->OnSerialData(cacheComport.data(), cacheComport.length());
    }
    cacheComport.clear();
}

void ComportDialog::OnSerialReadyRead()
{
    cacheComport.append(port->readAll());
    timer->start(ui->spinBox_byteTimeout->value());
}

//---------------------------------------------------------------------------------------
void ComportDialog::write(ComChn chn, const void *writeData, int len)
{
    const char* _data = (const char*)writeData;

    frameLog->FrameLog(FrameLogInf::SEND, _data, len);
    switch (chn)
    {
    case ComChnTCP: tcpSocket->write(_data, len); break;
    case ComChnUDP:
        udpSocket->writeDatagram(_data, len,
                                 QHostAddress(ui->lineEditRemoteIP->text()),
                                 ui->spinBoxRemotePort->value());
        break;
    case ComChnSerial: port->write(_data, len); break;
    }
}
//---------------------------------------------------------------------------------------
void ComportDialog::on_checkBoxAutoLocalPort_toggled(bool checked)
{
    ui->spinBoxLocalPort->setEnabled(!checked);
}

void ComportDialog::UpdateNetUi()
{
    bool uiEn, done;

    switch (ui->comboBoxNetType->currentIndex())
    {
    case 0:
        uiEn = !(tcpSocket->state() == QAbstractSocket::ConnectedState);
        done = (tcpSocket->state() == QAbstractSocket::UnconnectedState
                || tcpSocket->state() == QAbstractSocket::ConnectedState);
        break;
    case 1:
    default:
        uiEn = !udpSocketConnected;
        done = true;
        if (udpSocketConnected)
            ui->spinBoxLocalPort->setValue(udpSocket->localPort());
        break;
    }

    ui->pushButtonConnect->setEnabled(done);

    ui->groupBoxNetWork->setEnabled(uiEn);
    ui->pushButtonConnect->setText(uiEn ? tr("连接"):tr("断开"));
}

void ComportDialog::OnTcpConnected()
{
    UpdateNetUi();
    ui->spinBoxLocalPort->setValue(tcpSocket->localPort());
}
void ComportDialog::OnTcpDisconnected()
{
    UpdateNetUi();
}
void ComportDialog::OnTcpError()
{
    QMessageBox::critical(this, "Socket", tcpSocket->errorString());
    if (tcpSocket->state() == QAbstractSocket::ConnectedState)
        tcpSocket->disconnectFromHost();
    OnTcpDisconnected();
}

void ComportDialog::OnTcpReadyRead()
{
    qDebug() << "Got Data From Tcp" << endl;

    QByteArray udpCache = tcpSocket->readAll();
    frameLog->FrameLog(FrameLogInf::RECEIVE, udpCache.data(), udpCache.length());
    foreach (ComInf *sub, subs)
    {
        sub->OnTcpData(cacheComport.data(), cacheComport.length());
    }
}
void ComportDialog::OnUdpReadyRead()
{
    qDebug() << "Got Data From Udp" << endl;

    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        frameLog->FrameLog(FrameLogInf::RECEIVE, datagram.data(), datagram.length());
        foreach (ComInf *sub, subs)
        {
            sub->OnUdpData(cacheComport.data(), cacheComport.length());
        }
    }
}

int ComportDialog::DoBindPort(QAbstractSocket *socket)
{
    qint16 localPort = ui->checkBoxAutoLocalPort->isChecked()
            ? 0 : ui->spinBoxLocalPort->value();

    if (!socket->bind(QHostAddress::Any, localPort,
                      QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint))
    {
        QMessageBox::critical(this, "Socket", tr("Bind Port[%1] Failed!!!")
                              .arg(localPort));
        return -1;
    }
    return 0;
}

void ComportDialog::DoTcpConnect()
{
    QAbstractSocket *socket = tcpSocket;

    switch (socket->state())
    {
    case QAbstractSocket::UnconnectedState:
        if (DoBindPort(tcpSocket) < 0)
            return;
        socket->connectToHost(QHostAddress(ui->lineEditRemoteIP->text()),
                              ui->spinBoxRemotePort->value());
        UpdateNetUi();
        break;
    case QAbstractSocket::ConnectedState:
        socket->disconnectFromHost();
        break;
    default:
        QMessageBox::warning(this, "Socket", "Socket is busy!");
        break;
    }
}
void ComportDialog::DoUdpConnect()
{
    if (udpSocketConnected)
    {
        udpSocket->close();
        udpSocketConnected = false;
    }
    else
    {
        if (DoBindPort(udpSocket) < 0)
            return;
        udpSocketConnected = true;
    }

    UpdateNetUi();
}

void ComportDialog::on_pushButtonConnect_clicked()
{
    switch (ui->comboBoxNetType->currentIndex())
    {
    case 0: DoTcpConnect(); break;
    case 1:
    default: DoUdpConnect(); break;
    }
}
//---------------------------------------------------------------------------------------
