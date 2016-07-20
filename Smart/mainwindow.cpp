#include <QtCore>
#include <QtWidgets>
#include "Comportdialog.h"
#include "Commfunc.h"
#include "about.h"
#include "TaskMan.h"
#include "SmartFrame.h"
#include "hled.h"
#include "errorcode.h"
#include "CommApp/BaseApp.h"
#include "CommApp/CommApp.h"
#include "CommApp/UpdateApp.h"
#include "CommApp/SuccessRatioApp.h"
#include "DevMan/ModemSetting.h"
#include "DevMan/DevManDialog.h"
#include "DevMan/DevSetWidget.h"
#include "Tools/ToolAscStr.h"
#include "Tools/TestCase.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//---------------------------------------------------------------------------------------
enum
{
    FRAMELOG_COLDATE, FRAMELOG_COLDIR, FRAMELOG_COLDATA,
};
//---------------------------------------------------------------------------------------
struct option
{
    QString link;
    ComWidget *widget;
    ComWidget *(*CreateApp)(QWidget *parent, TaskMan *task_man);
};
static option options[] =
{
    {"通用功能",    NULL, CreateCommApp},
    {"批量设备操作", NULL, CreateDevSetApp},
    {"通讯成功率",  NULL, CreateSuccessRatio},
    {"远程升级",    NULL, CreateUpdateApp},
};
//---------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetupAction();
    SetupStatusBar();

    com = new ComportDialog(this, this);
    connect(com, SIGNAL(sig_serial_open(bool,QString&)), SLOT(slotCommOpen(bool, QString&)));

    task_man = new TaskMan(com, this);
    com->AddSubscriber(task_man);
    modem_setting = new ModemSetting(task_man, this);

    about = new About(this);
    testCaseDialog = new TestCaseDialog(this);
    connect(testCaseDialog, &TestCaseDialog::SigTestCaseChanged,
            this, &MainWindow::OnTestCaseInfoChange);

    setupFunctionList();
    SetupTimer();
    setupTextBrowser();
    SetupRunEnv();

    OnTestCaseInfoChange();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete modem_setting;
    delete com;
}
//---------------------------------------------------------------------------------------
void MainWindow::SetupRunEnv()
{
    ui->splitter_h->setStretchFactor(0, 1);
    ui->splitter_h->setStretchFactor(1, 2);
    QTreeWidgetItem *item = ui->treeWidget->topLevelItem(0);
    on_treeWidget_itemClicked(item, 0);
    //showMaximized();
    com->exec();
}
//---------------------------------------------------------------------------------------
void MainWindow::SetupAction()
{
    actComSet = new QAction(QIcon(":/File/images/comport.png"), "通信参数", this);
    connect(actComSet, SIGNAL(triggered(bool)), SLOT(OnActCommportTriggered()));

    actModem = new QAction(QIcon(":/File/images/stm32.ico"), "抄控器设置", this);
    connect(actModem, SIGNAL(triggered(bool)), SLOT(OnActModemTriggered()));

    actDevMan = new QAction(QIcon(":/File/images/Smart_devices.ico"), "设备列表维护", this);
    connect(actDevMan, SIGNAL(triggered(bool)), SLOT(OnActDevManTriggered()));

    actQuit = new QAction(QIcon(":/File/images/quit.png"), "退出", this);
    connect(actQuit, SIGNAL(triggered(bool)), SLOT(close()));

    actAbout = new QAction(QIcon(":/File/images/Help.png"), "关于", this);
    connect(actAbout, SIGNAL(triggered(bool)), SLOT(OnActAboutTriggered()));

    actAscTool = new QAction("数据转换工具", this);
    connect(actAscTool, SIGNAL(triggered(bool)), SLOT(OnActAscTriggered()));
    actTestCase = new QAction("测试用例维护", this);
    connect(actTestCase, SIGNAL(triggered(bool)), SLOT(OnActTestCaseTriggered()));

    ui->menuSet->addAction(actComSet);
    ui->menuSet->addAction(actModem);
    ui->menuSet->addAction(actDevMan);
    ui->menuSet->addAction(actQuit);
    ui->menuTools->addAction(actAscTool);
    ui->menuTools->addAction(actTestCase);
    ui->menuHelp->addAction(actAbout);

    ui->toolbar->addAction(actComSet);
    ui->toolbar->addAction(actModem);
    ui->toolbar->addAction(actDevMan);
    ui->toolbar->addAction(actQuit);
}
//---------------------------------------------------------------------------------------
void MainWindow::SetupStatusBar()
{
    led = new HLed(this);
    ui->statusBar->addWidget(led, 0);
    led->turnOff();

    statusLblComm = new QLabel("COM3,9600,8N1", this);
    statusLblComm->setAlignment(Qt::AlignHCenter);
    statusLblComm->setToolTip("通讯端口信息");
    statusLblComm->setMinimumSize(statusLblComm->sizeHint());
    ui->statusBar->addWidget(statusLblComm, 0);

    tipsLabel = new QLabel(this);
    ui->statusBar->addWidget(tipsLabel, 1);

    statusLblCurTime = new QLabel(
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), this);
    statusLblCurTime->setAlignment(Qt::AlignHCenter);
    statusLblCurTime->setMinimumSize(statusLblCurTime->sizeHint());
    ui->statusBar->addPermanentWidget(statusLblCurTime);
}
//---------------------------------------------------------------------------------------
void MainWindow::SetupTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnTimerUpdate()));
    timer->start(1000);
}
void MainWindow::OnTimerUpdate()
{
    now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss zzz");
    statusLblCurTime->setText(now);

    for (size_t i = 0; i < array_size(options); ++i)
    {
        //if (options[i].widget)
        //    options[i].widget->on_timer_second();
    }
}
//---------------------------------------------------------------------------------------
void MainWindow::slotCommOpen(bool open, QString &text)
{
    led->turnOn(open);
    statusLblComm->setText(text);
}

void MainWindow::OnActCommportTriggered()
{
    com->exec();
}
void MainWindow::OnActModemTriggered()
{
    modem_setting->exec();
}
void MainWindow::OnActDevManTriggered()
{
    DevManDialog *d = new DevManDialog(this);
    d->exec();
    delete d;
}
void MainWindow::OnActAscTriggered()
{
    //QProcess::startDetached("tools.exe",QStringList());
    static ToolAscStrDialog *toolAscStrDialog = nullptr;

    if (!toolAscStrDialog) toolAscStrDialog = new ToolAscStrDialog(this);
    toolAscStrDialog->show();
}
void MainWindow::OnActTestCaseTriggered()
{
    testCaseDialog->exec();
}
void MainWindow::OnActAboutTriggered()
{
    about->exec();
}
//---------------------------------------------------------------------------------------
void MainWindow::OnTestCaseInfoChange(void)
{
    if (GetTestCaseInfoAll(testCaseInfoList) < 0)
        return;

    for (size_t i = 0; i < array_size(options); ++i)
    {
        if (options[i].widget)
            options[i].widget->OnTestCaseChanged(&testCaseInfoList);
    }
}
//---------------------------------------------------------------------------------------
void MainWindow::OnSaveToFile()
{
    QFile outfile;
    outfile.setFileName(QString("%1.log").arg(QDateTime::currentDateTime()
                                              .toString("yyyyMMdd")));
    outfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&outfile);

    int rowCnt = ui->tableWidgetFrame->rowCount();
    for (int i = 0; i < rowCnt; i++)
    {
        out << ui->tableWidgetFrame->item(i, FRAMELOG_COLDATE)->text()
            << "[" << ui->tableWidgetFrame->item(i, FRAMELOG_COLDIR)->text() << "]"
            << ui->tableWidgetFrame->item(i, FRAMELOG_COLDATA)->text()
            << endl;
    }
    outfile.close();
    QMessageBox::information(this, "恭喜",
                             QString("保存[%1]完成").arg(outfile.fileName()));
}
void MainWindow::OnClearTableWidget(void)
{
    int rows = ui->tableWidgetFrame->rowCount();

    while (rows--)
    {
        ui->tableWidgetFrame->removeRow(0);
    }
}
void MainWindow::setupTextBrowser()
{
#if 0
    QAction *action_clear = new QAction("Clear", ui->textBrowser);
    connect(action_clear, SIGNAL(triggered()), ui->textBrowser, SLOT(clear()));
    ui->textBrowser->addAction(action_clear);

    QAction *action_save = new QAction("Save", ui->textBrowser);
    connect(action_save, SIGNAL(triggered()), this, SLOT(slot_save_to_file()));
    ui->textBrowser->addAction(action_save);

    ui->textBrowser->setContextMenuPolicy(Qt::ActionsContextMenu);
#endif

    QAction *actionClear = new QAction("Clear", ui->tableWidgetFrame);
    connect(actionClear, SIGNAL(triggered()), this, SLOT(OnClearTableWidget()));
    ui->tableWidgetFrame->addAction(actionClear);

    QAction *actionSave = new QAction("Save", ui->tableWidgetFrame);
    connect(actionSave, SIGNAL(triggered()), this, SLOT(OnSaveToFile()));
    ui->tableWidgetFrame->addAction(actionSave);

    ui->tableWidgetFrame->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableWidgetFrame->resizeColumnsToContents();
    ui->tableWidgetFrame->resizeRowsToContents();
}
//---------------------------------------------------------------------------------------
void MainWindow::setupFunctionList()
{
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabel("功能列表");

    for (size_t i = 0; i < array_size(options); i++)
    {
        option *opt = &options[i];
        QString optLink = opt->link;

        QStringList children = optLink.split("/");
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget,
                                                    QStringList(children.at(0)));
        ui->treeWidget->addTopLevelItem(item);
#if 0
        for (int j = 1; j < children.count(); j++)
        {
            item = new QTreeWidgetItem(item, QStringList(children.at(j)));
        }
#endif
    }

    for (size_t i = 0; i < array_size(options); i++)
    {
        if (!options[i].widget)
        {
            options[i].widget = options[i].CreateApp(this, task_man);
            ui->gridLayout_app->addWidget(options[i].widget);
            options[i].widget->hide();
        }
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    for (size_t i = 0; i < array_size(options); i++)
    {
        if (options[i].widget)
            options[i].widget->hide();
    }

    QTreeWidgetItem *parent=item->parent();//获得父节点
    if (!parent)
    {
        int row = ui->treeWidget->indexOfTopLevelItem(item);
        ui->groupBox_app->setTitle(item->text(0));
        options[row].widget->show();
    }
#if 0
    int row=parent->indexOfChild(item);//获得节点在父节点中的行号(从0开始)
    QString fileName=parent->text(0);//获得父节点的文本字符(即影像路径)
#endif
}
//---------------------------------------------------------------------------------------
void MainWindow::Log(const QString &log)
{
    QString str = QString("%1[%2]").arg(now).arg(log);

    ui->textBrowser->append(str);
}

void MainWindow::on_tableWidgetFrame_clicked(const QModelIndex &index)
{
    bool localFrame = false;
    int didx = 0;
    int row = index.row();
    QString desp;
    QTableWidgetItem *item;

    ui->textBrowserFrameInfo->clear();
    item = ui->tableWidgetFrame->item(row, FRAMELOG_COLDATE);
    ui->textBrowserFrameInfo->append(QString("时间：%1").arg(item->text()));
    item = ui->tableWidgetFrame->item(row, FRAMELOG_COLDIR);
    ui->textBrowserFrameInfo->append(QString("方向：%1").arg(item->text()));

    item = ui->tableWidgetFrame->item(row, FRAMELOG_COLDATA);
    uint8_t tmp[0x200];
    char str[0x200];
    int len = str2arr(item->text().toLatin1(), tmp, sizeof(tmp));
    SmartFrame *frame = get_smart_frame(tmp, len);
    if (!frame) return;

    ui->textBrowserFrameInfo->append(QString("源址：%1")
                                     .arg(get_le_val(frame->said, sizeof(frame->said))));

    ui->textBrowserFrameInfo->append(QString("目的：%1")
                                     .arg(get_le_val(frame->taid, sizeof(frame->taid))));
#if 0
    ui->textBrowserFrameInfo->append(QString("序号：%1")
                                     .arg(QString::number(frame->seq, 16)));
    ui->textBrowserFrameInfo->append(QString("长度：%1")
                                     .arg(QString::number(frame->len, 16)));
#endif
    didx = 0;
    localFrame = is_local_frame(frame);

    if (frame->len < 1) goto out;
    desp = GetCmdDesp(localFrame, frame->data[didx]);
    if (desp.isEmpty()) desp = QString::number(frame->data[didx], 16);
    ui->textBrowserFrameInfo->append(QString("命令：%1").arg(desp));

    didx += 1;
    if (!localFrame)
    {
        if (frame->len < didx+DID_LEN) goto out;
        int did = get_le_val(&frame->data[didx], DID_LEN);
        snprintf(str, sizeof(str), "%04X", did);
        ui->textBrowserFrameInfo->append(QString("标识：%1").arg(str));
        didx += DID_LEN+1;
    }
    desp = arr2str(&frame->data[didx], frame->len - didx, str, sizeof(str));
    if (desp.isEmpty()) desp = "无";
    ui->textBrowserFrameInfo->append(QString("数据：%1").arg(desp));
    didx = frame->len;

out:
#if 0
    ui->textBrowserFrameInfo->append(QString("校验：%1")
                                     .arg(QString::number(frame->data[didx], 16)));
#endif
    ;
}
void MainWindow::FrameLog(FrameLogInf::DIR dir, const void *data, int len)
{
    char tmp[0x1000];

    SmartFrame *frame = get_smart_frame((const uint8_t *)data, len);
    if (!frame) return;

    arr2str(frame->said, sizeof(frame->said), tmp, sizeof(tmp));

    int row = ui->tableWidgetFrame->rowCount();

    ui->tableWidgetFrame->insertRow(row);
    ui->tableWidgetFrame->setItem(row, FRAMELOG_COLDATE, new QTableWidgetItem(now));
    ui->tableWidgetFrame->setItem(row, FRAMELOG_COLDIR,
                                  new QTableWidgetItem(dir==FrameLogInf::SEND?"<--":"-->"));
    arr2str(data, len, tmp, sizeof(tmp));
    ui->tableWidgetFrame->setItem(row, FRAMELOG_COLDATA, new QTableWidgetItem(tmp));

    //ui->tableWidgetFrame->scrollToBottom();
#if 0
    QFile outfile;
    outfile.setFileName(QString("Frame%1.log").arg(QDateTime::currentDateTime()
                                                    .toString("yyyyMMdd")));
    outfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&outfile);

    QString str = QString("%1[%2]").arg(now).arg(arr2str(data, len, tmp, sizeof(tmp)));
    out << str << endl;
    outfile.close();
#endif
}

void MainWindow::ShowStatus(const QString &status)
{
    tipsLabel->setText(status);
}
//---------------------------------------------------------------------------------------
#if 0
void MainWindow::AddFrameInfo(const struct FrameInfo *info)
{
    QString str(now);

    if (info->dir)

    str = str + "[" + prefix + "]: " + log;
    ui->textBrowser->append(str);
}
#endif
//---------------------------------------------------------------------------------------

