#include <QtWidgets>
#include "errorcode.h"
#include "CommFunc.h"
#include "GlobalSetting.h"
#include "TaskMan.h"
#include "ModemSetting.h"
#include "ui_ModemSetDialog.h"

//---------------------------------------------------------------------------------------
ModemSetting::ModemSetting(TaskMan *task_man, QWidget *parent) :
    QDialog(parent), ui(new Ui::ModemSetDialog), task_man(task_man)
{
    ui->setupUi(this);

    task = new Task();
    setting = GlobalSetting::GetInstance();

    connect(ui->pushButtonSetID,  SIGNAL(clicked(bool)), SLOT(OnBtnClicked()));
    connect(ui->pushButtonGetVer, SIGNAL(clicked(bool)), SLOT(OnBtnClicked()));
    connect(ui->pushButtonSetKey, SIGNAL(clicked(bool)), SLOT(OnBtnClicked()));
    connect(ui->pushButtonSetBaud,SIGNAL(clicked(bool)), SLOT(OnBtnClicked()));
}

ModemSetting::~ModemSetting()
{
    delete task;
}
//---------------------------------------------------------------------------------------
void ModemSetting::slot_accept(void)
{
    accept();
}

//---------------------------------------------------------------------------------------
void ModemSetting::OnBtnClicked()
{
    uint8_t val[AID_LEN];
    if (QObject::sender() == ui->pushButtonSetID)
    {
        put_le_val(ui->lineEditID->text().toInt(), val, AID_LEN);
        task->dlen = code_local_frame(CMD_SET_AID, val, AID_LEN, task->data, sizeof(task->data));
    }
    else if (QObject::sender() == ui->pushButtonSetKey)
    {
        put_le_val(ui->lineEditKey->text().toInt(), val, SID_LEN);
        task->dlen = code_local_frame(CMD_SET_PANID, val, SID_LEN, task->data, sizeof(task->data));
    }
    else if (QObject::sender() == ui->pushButtonGetVer)
    {
        task->dlen = code_local_frame(CMD_GET_VER, NULL, 0, task->data, sizeof(task->data));
    }
    else if (QObject::sender() == ui->pushButtonSetBaud)
    {
        val[0] = ui->comboBoxBaud->currentIndex();
        val[1] = 0;
        task->dlen = code_local_frame(CMD_SET_BPS, val, 2, task->data, sizeof(task->data));
    }

    task->args = QObject::sender();
    task->taskInf = this;
    int err = task_man->AddTask(task);
    if (err < 0)
    {
        QMessageBox::warning(this, "错误", SmartError(-err));
    }
}
void ModemSetting::OnTaskDone(Task *task)
{
    if (task->dlen <= 0)
    {
        QMessageBox::warning(this, "错误", "通讯失败");
        return;
    }

    if (is_local_frame((struct SmartFrame*)task->data))
    {
        struct SmartFrame *frame = (struct SmartFrame*)task->data;
        int cmd = get_local_ack_cmd(frame);
        if (cmd == CMD_ACK || cmd == CMD_NAK)
        {
            QMessageBox::information(this, "恭喜", "设置成功");
            if (task->args == ui->pushButtonSetID)
                setting->master_id = ui->lineEditID->text().toInt();
            else if (task->args == ui->pushButtonSetBaud)
                task_man->com->SetBaud(ui->comboBoxBaud->currentText().toInt());
        }
        else if (cmd == CMD_ACK_VER)
        {
            frame->data[frame->len] = '\0';
            ui->lineEditVer->setText((char*)&frame->data[1]);
        }
    }
}
