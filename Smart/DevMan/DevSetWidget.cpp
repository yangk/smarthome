#include <QSqlQuery>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QMessageBox>
#include <QDebug>
#include "../TaskMan.h"
#include "DevSetWidget.h"
#include "Commfunc.h"
#include "Database/ddm.h"
#include "../GlobalSetting.h"
#include "../errorcode.h"
#include "ui_DevSetWidget.h"

//---------------------------------------------------------------------------------------
enum
{
    STATUS_IDLE, STATUS_RUNING, STATUS_STOPING,
};
enum
{
    COL_AID, COL_PASSWD, COL_PANID, COL_GROUPNO, COL_KEY, COL_STATUS
};
enum
{
    RADIO_GETVER, RADIO_PANID, RADIO_GROUPNO, RADIO_KEY
};

struct DevSetArg
{
    int row;
    int setType;
    uint16_t did;
};

//---------------------------------------------------------------------------------------
DevSetWidget::DevSetWidget(TaskMan *task_man, QWidget *parent) :
     ComWidget(parent), ui(new Ui::DevSet), task_man(task_man)
{
    ui->setupUi(this);
    SetupAction();
    SetupBtnGroup();
    ui->radioButtonGetVer->setChecked(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setting = GlobalSetting::GetInstance();
    devArg = new DevSetArg();
    task = new Task();
    status = STATUS_IDLE;
}
void DevSetWidget::SetupAction(void)
{
    actDevList = ui->buttonBox->addButton("导入预设设备列表", QDialogButtonBox::ActionRole);
    connect(actDevList, SIGNAL(clicked(bool)), SLOT(OnActDevListTriggered()));

    actSelectAll = ui->buttonBox->addButton("全选", QDialogButtonBox::ActionRole);
    connect(actSelectAll, SIGNAL(clicked(bool)), SLOT(OnActSelectAllTriggered()));

    actDeSelect = ui->buttonBox->addButton("反选", QDialogButtonBox::ActionRole);
    connect(actDeSelect, SIGNAL(clicked(bool)), SLOT(OnActDeSelectTriggered()));

    actStart = ui->buttonBox->addButton("开始", QDialogButtonBox::ActionRole);
    connect(actStart, SIGNAL(clicked(bool)), SLOT(OnActStartTriggered()));
}

void DevSetWidget::SetupBtnGroup(void)
{
    btnGroup = new QButtonGroup(this);

    btnGroup->addButton(ui->radioButtonGetVer, RADIO_GETVER);
    btnGroup->addButton(ui->radioButtonPANID, RADIO_PANID);
    btnGroup->addButton(ui->radioButtonGroupNo, RADIO_GROUPNO);
    btnGroup->addButton(ui->radioButtonKey, RADIO_KEY);
}

DevSetWidget::~DevSetWidget()
{
    delete ui;
    delete task;
    delete devArg;
}
//---------------------------------------------------------------------------------------
void DevSetWidget::ClearTableWidget(void)
{
    int rows = ui->tableWidget->rowCount();

    while (rows--)
    {
        ui->tableWidget->removeRow(0);
    }
}

void DevSetWidget::OnActDevListTriggered()
{
    QSqlQuery q;

    QString sql(QObject::tr("SELECT AID, PASSWD, PANID, GROUP_NO, KEY FROM SmartDevices"));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << DDMError();
        return;
    }
    ClearTableWidget();

    int row = 0;
    while (q.next())
    {
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setCellWidget(row, COL_AID, new QCheckBox(q.value(0).toString()));
        ui->tableWidget->setItem(row, COL_PASSWD, new QTableWidgetItem(q.value(1).toString()));
        ui->tableWidget->setItem(row, COL_PANID,  new QTableWidgetItem(q.value(2).toString()));
        ui->tableWidget->setItem(row, COL_GROUPNO,new QTableWidgetItem(q.value(3).toString()));
        ui->tableWidget->setItem(row, COL_KEY,    new QTableWidgetItem(q.value(4).toString()));
        ui->tableWidget->setItem(row, COL_STATUS, new QTableWidgetItem());
        row++;
    }
    OnActSelectAllTriggered();
}
//---------------------------------------------------------------------------------------
void DevSetWidget::UpdateUI(void)
{
    switch (status)
    {
    case STATUS_IDLE:
        ui->tableWidget->setEnabled(true);
        ui->groupBox->setEnabled(true);
        actStart->setText("开始");
        actStart->setEnabled(true);
        break;
    case STATUS_RUNING:
        ui->tableWidget->setEnabled(false);
        ui->groupBox->setEnabled(false);
        actStart->setText("停止");
        actStart->setEnabled(true);
        break;
    case STATUS_STOPING:
        ui->tableWidget->setEnabled(false);
        ui->groupBox->setEnabled(false);
        actStart->setText("停止中");
        actStart->setEnabled(false);
        break;
    }
}

//---------------------------------------------------------------------------------------
ComWidget *CreateDevSetApp(QWidget *parent, TaskMan *task_man)
{
    return new DevSetWidget(task_man, parent);
}
//---------------------------------------------------------------------------------------
void DevSetWidget::OnActSelectAllTriggered()
{
    int rows = ui->tableWidget->rowCount();

    while (rows--)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(rows, COL_AID));
        checkBox->setChecked(true);
    }
}
void DevSetWidget::OnActDeSelectTriggered()
{
    int rows = ui->tableWidget->rowCount();

    while (rows--)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(rows, COL_AID));
        checkBox->setChecked(!checkBox->isChecked());
    }
}
void DevSetWidget::ClearTableStatus(void)
{
    int rows = ui->tableWidget->rowCount();

    while (rows--)
    {
        QTableWidgetItem *item = ui->tableWidget->item(rows, COL_STATUS);
        item->setText("");
    }
}
//---------------------------------------------------------------------------------------
int DevSetWidget::GetNextRow(int from)
{
    int rows = ui->tableWidget->rowCount();

    for (int i = from; i < rows; i++)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, COL_AID));
        if (checkBox->isChecked())
        {
            return i;
        }
    }
    return -1;
}
//---------------------------------------------------------------------------------------
void DevSetWidget::OnTaskDone(Task *task)
{
    QString sucMsg("操作成功"), errMsg("操作失败");
    QString msg;

    struct DevSetArg *arg = (struct DevSetArg *)task->args;
    int row = arg->row;
    if (task->dlen > 0)
    {
        struct Body *body;
        switch(arg->setType)
        {
        case RADIO_GETVER:
            body = (struct Body *)GetBody((struct SmartFrame *)task->data, DID_INTVER);
            body->data[get_bits(body->ctrl, 0, 6)] = '\0';
            msg = QString((char*)body->data);
            break;
        case RADIO_PANID:
            body = (struct Body *)GetBody((struct SmartFrame *)task->data, DID_REG);
            msg = (body && !tst_bit(body->ctrl, 7)) ? sucMsg : errMsg;
            break;
        case RADIO_GROUPNO:
            body = (struct Body *)GetBody((struct SmartFrame *)task->data, DID_GROUPNO);
            msg = (body && !tst_bit(body->ctrl, 7)) ? sucMsg : errMsg;
            break;
        case RADIO_KEY:
            body = (struct Body *)GetBody((struct SmartFrame *)task->data, DID_KEY);
            msg = (body && !tst_bit(body->ctrl, 7)) ? sucMsg : errMsg;
            break;
        }
    }
    else
    {
        msg = errMsg;
    }

    if (status == STATUS_RUNING)
    {
        int next = GetNextRow(row+1);
        if (next < 0)
        {
            status = STATUS_IDLE;
            goto out;
        }
        DoSet(next);
    }
    else if (status == STATUS_STOPING)
    {
        status = STATUS_IDLE;
    }

out:
    UpdateUI();
    QTableWidgetItem *item = ui->tableWidget->item(row, COL_STATUS);
    item->setText(msg);
}
//---------------------------------------------------------------------------------------
int DevSetWidget::DoSet(int row)
{
    uint8_t val[8];
    memset(val, 0, sizeof(val));

    QCheckBox *checkBox = dynamic_cast<QCheckBox*>(ui->tableWidget->cellWidget(row, COL_AID));
    uint32_t dest = checkBox->text().toInt();
    switch(btnGroup->checkedId())
    {
    case RADIO_GETVER:
        task->dlen = CodeUnicastFrame(setting->master_id, dest,
                                      CMD_READ, DID_INTVER, NULL, 0,
                                      task->data, sizeof(task->data));
        break;
    case RADIO_PANID:
    {
        struct RegData reg;
        put_le_val(dest, reg.aid, sizeof(reg.aid));
        put_le_val(ui->tableWidget->item(row, COL_PANID)->text().toInt(), reg.panid, sizeof(reg.panid));
        put_le_val(ui->tableWidget->item(row, COL_PASSWD)->text().toInt(), reg.pw, sizeof(reg.pw));
        put_le_val(setting->master_id, reg.gid, sizeof(reg.gid));
        put_le_val(0x01, reg.sid, sizeof(reg.sid));
        task->dlen = CodeUnicastFrame(setting->master_id, dest,
                                      CMD_SET, DID_REG, &reg, sizeof(reg),
                                      task->data, sizeof(task->data));
        break;
    }
    case RADIO_GROUPNO:
        put_le_val(ui->tableWidget->item(row, COL_GROUPNO)->text().toInt(), val, SID_LEN);
        task->dlen = CodeUnicastFrame(setting->master_id, dest,
                                      CMD_SET, DID_GROUPNO, val, SID_LEN,
                                      task->data, sizeof(task->data));
        break;
    case RADIO_KEY:
        str2arr(ui->tableWidget->item(row, COL_KEY)->text().toLatin1(), val, sizeof(val));
        task->dlen = CodeUnicastFrame(setting->master_id, dest,
                                      CMD_SET, DID_KEY, val, 8,
                                      task->data, sizeof(task->data));
        break;
    default:
        QMessageBox::critical(this, tr("错误"), tr("请选择设置类型"));
        return -1;
    }
    devArg->row = row;
    devArg->setType = btnGroup->checkedId();
    task->args = devArg;
    task->taskInf = this;
    int err = task_man->AddTask(task);
    if (err < 0)
    {
        QMessageBox::critical(this, tr("错误"), SmartError(-err));
        return -1;
    }
    return 0;
}
//---------------------------------------------------------------------------------------
void DevSetWidget::OnActStartTriggered()
{
    if (status == STATUS_IDLE)
    {
        int row = GetNextRow(0);
        if (row < 0)
        {
            QMessageBox::critical(this, tr("错误"), tr("请指定所控设备"));
            return;
        }

        ClearTableStatus();

        if (DoSet(row) < 0)
            return;

        status = STATUS_RUNING;
    }
    else if (status == STATUS_RUNING)
    {
        status = STATUS_STOPING;
    }

    UpdateUI();
}
//---------------------------------------------------------------------------------------
