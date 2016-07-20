#include <QtWidgets>
#include "SmartFrame.h"
#include "errorcode.h"
#include "Commfunc.h"
#include "GlobalSetting.h"
#include "TaskMan.h"
#include "Database/ddm.h"
#include "SendGroup.h"
#include "ui_SendGroupWidget.h"

//---------------------------------------------------------------------------------------
static const uint8_t cmdArray[] = {CMD_READ, CMD_SET};
//---------------------------------------------------------------------------------------
SendGroup::SendGroup(TaskMan *taskMan, QWidget *parent)
    : QWidget(parent), ui(new Ui::SendGroupWidget), taskMan(taskMan)
{
    ui->setupUi(this);

    connect(ui->pushButtonSend, &QPushButton::clicked, this, &SendGroup::OnBtnSendClicked);
    connect(ui->pushButtonSendRaw, &QPushButton::clicked, this, &SendGroup::OnBtnSendClicked);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &SendGroup::OnTimeout);

    on_checkBoxRaw_toggled(false);
    on_checkBoxCycle_toggled(false);

    setting = GlobalSetting::GetInstance();
    task = new Task();
    cycleSending = false;
    ui->lineEditDest->setText(QString::number(setting->devid));
}
SendGroup::~SendGroup()
{
    delete ui;
    delete task;
}
//---------------------------------------------------------------------------------------
void SendGroup::SetTitle(const QString &title)
{
    ui->groupBoxAutoSum->setTitle(title);
}
void SendGroup::OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList)
{
    this->testCaseInfoList = testCaseInfoList;
    disconnect(ui->comboBoxTips, SIGNAL(currentIndexChanged(int)),
               this, SLOT(on_comboBoxTips_currentIndexChanged(int)));
    ui->comboBoxTips->clear();
    for (int i = 0; i != testCaseInfoList->count(); i++)
    {
        ui->comboBoxTips->addItem(testCaseInfoList->at(i).tips);
    }
    if (testCaseInfoList->count() > 0)
        on_comboBoxTips_currentIndexChanged(0);
    connect(ui->comboBoxTips, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_comboBoxTips_currentIndexChanged(int)));
}

//---------------------------------------------------------------------------------------
void SendGroup::on_checkBoxRaw_toggled(bool checked)
{
    bool raw = checked;

    ui->lineEditDest->setHidden(raw);
    ui->comboBoxCmd->setHidden(raw);
    ui->lineEditDID->setHidden(raw);
    ui->lineEditData->setHidden(raw);
    ui->pushButtonSend->setHidden(raw);

    ui->lineEditRaw->setHidden(!raw);
    ui->checkBoxAutoSum->setHidden(!raw);
    ui->pushButtonSendRaw->setHidden(!raw);
}
void SendGroup::on_checkBoxCycle_toggled(bool checked)
{
    ui->spinBoxCycleTime->setEnabled(checked);
    if (!checked)
    {
        ui->pushButtonSend->setText("发送");
        ui->pushButtonSendRaw->setText("发送");
        timer->stop();
    }
}
//---------------------------------------------------------------------------------------
int SendGroup::GetFrameFromUi(uint8_t *out, int maxlen)
{
    uint8_t data[0x100];

    uint32_t dest = ui->lineEditDest->text().toUInt();
    int cmd = cmdArray[ui->comboBoxCmd->currentIndex()];
    int did = ui->lineEditDID->text().toInt(NULL, 16);
    int dlen = str2arr(ui->lineEditData->text().toLatin1(), data, sizeof(data));

    return CodeUnicastFrame(setting->master_id, dest, cmd, did, data, dlen, out, maxlen);
}
int SendGroup::GetFrameFromRawUi(uint8_t *out, int maxlen)
{
    int len = str2arr(ui->lineEditRaw->text().toLatin1(), out, maxlen-1);
    if (ui->checkBoxAutoSum->isChecked())
    {
        out[len] = checksum(out, len);
        len++;
    }
    return len;
}
//---------------------------------------------------------------------------------------
void SendGroup::OnTaskDone(Task *task)
{
    if (task->dlen > 0)
    {
        char buf[0x200];
        arr2str(task->data, task->dlen, buf, sizeof(buf));
        ui->lineEditReturn->setText(buf);
    }
    else
    {
        ui->lineEditReturn->setText(SmartError(-task->dlen));
    }

    if (ui->checkBoxCycle->isChecked() && cycleSending)
        timer->start(ui->spinBoxCycleTime->value());

    emit SigGetReply(task->dlen > 0);
}
int SendGroup::DoTask(void)
{
    if (ui->checkBoxRaw->isChecked())
        task->dlen = GetFrameFromRawUi(task->data, sizeof(task->data));
    else
        task->dlen = GetFrameFromUi(task->data, sizeof(task->data));

    if (task->dlen < 0) return task->dlen;

    ui->lineEditReturn->setText("");
    task->taskInf = this;
    int err = taskMan->AddTask(task);
    if (err < 0) QMessageBox::warning(this, "错误", SmartError(-err));
    setting->devid = ui->lineEditDest->text().toUInt();
    return err;
}
//---------------------------------------------------------------------------------------
void SendGroup::OnBtnSendClicked()
{
    QPushButton *btn = dynamic_cast<QPushButton*>(QObject::sender());

    if (btn->text() == "发送")
    {
        if (DoTask() < 0) return;
        if (ui->checkBoxCycle->isChecked())
        {
            cycleSending = true;
            btn->setText("停止");
        }
    }
    else
    {
        if (cycleSending)
        {
            if (timer->isActive()) timer->stop();
            cycleSending = false;
            btn->setText("发送");
        }
    }
}
void SendGroup::OnTimeout(void)
{
    if (ui->checkBoxCycle->isChecked())
        DoTask();
}
//---------------------------------------------------------------------------------------
void SendGroup::on_comboBoxTips_currentIndexChanged(int index)
{
    QString cmd, DID, data;

    if (GetTestCaseByID(testCaseInfoList->at(index).id, DID, cmd, data) < 0)
        return;

    ui->lineEditDID->setText(DID);
    ui->comboBoxCmd->setCurrentText(cmd);
    ui->lineEditData->setText(data);
}
//---------------------------------------------------------------------------------------
void SendGroup::on_pushButtonSave_clicked()
{
    TestCaseInfo info;

    info.tips = QInputDialog::getText(this, "输入", "请输入该报文描述");
    if (info.tips.isEmpty()) return;

    info.id = AddTestCase(ui->lineEditDID->text(),
                          ui->comboBoxCmd->currentText(),
                          ui->lineEditData->text(),
                          info.tips);
    if (info.id < 0)
        return;

    if (testCaseInfoList)
    {
        testCaseInfoList->append(info);
        OnTestCaseChanged(testCaseInfoList);
    }
    QMessageBox::information(this, "恭喜", "增加成功");
}
//---------------------------------------------------------------------------------------
