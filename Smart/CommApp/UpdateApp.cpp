#include <assert.h>
#include <QtCore>
#include <QtWidgets>
#include <QSqlQuery>
#include "SmartFrame.h"
#include "errorcode.h"
#include "Commfunc.h"
#include "GlobalSetting.h"
#include "TaskMan.h"
#include "SendGroup.h"
#include "UpdateApp.h"
#include "Database/ddm.h"
#include "ui_UpdateWidget.h"

//-------------------------------------------------------------------------------------------------
#define UPDATE_FINISHED 0xFFFF
enum
{
    COL_AID, COL_STATUS,
};
struct UpdateArg
{
    uint32_t devID;
    int curRow;
};

//---------------------------------------------------------------------------------------
static const uint8_t update_cmd[] = {CMD_UPDATE_PLC, CMD_UPDATE_PLC, CMD_UPDATE};
//---------------------------------------------------------------------------------------
UpdateApp::UpdateApp(TaskMan *task_man, QWidget *parent) :
    ComWidget(parent), ui(new Ui::UpdateWidget), task_man(task_man)
{
    ui->setupUi(this);

    updateState = UPDATE_STATE_IDLE;
    blockNr = block_size = 0;
    setting = GlobalSetting::GetInstance();
    task = new Task();
    updateArg = new UpdateArg();
    UpdateUi();
}
UpdateApp::~UpdateApp()
{
    delete task;
    delete updateArg;
}
//---------------------------------------------------------------------------------------
const char *UpdateApp::GetInfoInHeader(const char *header, int hlen, const char *str)
{
    const char *find = (const char *)memmem(header, hlen, str, strlen(str));
    if (!find) return NULL;
    return find+strlen(str);
}

int UpdateApp::ParseUpdateFileHeader(const QString &filename)
{
    uint16_t tmpCrc, tmpSize;
    QFile f(filename);

    if (!f.open(QIODevice::ReadOnly)) return -1;
    updateData = f.readAll();
    f.close();

    char header[0x100];
    memcpy(header, updateData.data(), min((size_t)updateData.size(), sizeof(header)));
    header[sizeof(header)-1] = '\0';

    const char *_devtype, *_softver, *_crc, *_size, *start;

    if (!(_devtype = GetInfoInHeader(header, sizeof(header), "device type:")))
        goto format_err_lbl;
    if (!(_softver = GetInfoInHeader(header, sizeof(header), "soft ver:")))
        goto format_err_lbl;
    if (!(_crc = GetInfoInHeader(header, sizeof(header), "file crc:")))
        goto format_err_lbl;
    if (!(_size = GetInfoInHeader(header, sizeof(header), "file size:")))
        goto format_err_lbl;
    if (!(start = GetInfoInHeader(header, sizeof(header), "program start:")))
        goto format_err_lbl;

    devtype = QString(_devtype);
    softver = QString(_softver);
    crc = QString(_crc);
    size = QString(_size);

    int fileSize;

    ui->textBrowserFileInfo->clear();
    ui->textBrowserFileInfo->append(QString("设备类型：%1").arg(devtype));
    ui->textBrowserFileInfo->append(QString("软件版本：%1").arg(softver));
    ui->textBrowserFileInfo->append(QString("CRC校验：%1").arg(crc));
    ui->textBrowserFileInfo->append(QString("文件大小：%1").arg(size));

    updateData.remove(0, start - header);

    fileCrc = strtoul(_crc, NULL, 16);
    fileCrc = be16_to_cpu(fileCrc);
    tmpCrc = calc_crc16(0, updateData.data(), updateData.size());
    fileSize = strtoul(_size, NULL, 16);
    tmpSize = updateData.size();
    if (fileSize != tmpSize
        //|| fileCrc != tmpCrc)
        )
    {
        goto format_err_lbl;
    }

    return 0;

format_err_lbl:
    QMessageBox::information(this, "错误", "文件格式错误!");
    updateData.clear();
    return -1;
}
//---------------------------------------------------------------------------------------
void UpdateApp::on_pushButtonGetFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "请选择升级文件", ".",
                                                    "二进制文件(*.bin)");
    if (fileName.isEmpty()) return;

    ui->lineEditPath->setText(fileName);

    ParseUpdateFileHeader(fileName);
}
//---------------------------------------------------------------------------------------
int UpdateApp::CodeHeadPacket(const QByteArray &b, void *out, size_t maxlen)
{
    if (maxlen < sizeof(struct UpdateHeader) + softver.count())
        return -1;

    struct UpdateHeader *h = (struct UpdateHeader *)out;

    block_size = ui->comboBoxPacket->currentText().toInt();
    blockNr = (b.size() + block_size - 1) / block_size;
    put_le_val(b.size(), h->size, sizeof(h->size));
    put_le_val(fileCrc, h->crc, sizeof(h->crc));
    h->block_size = block_size;
    str2arr(devtype.toLatin1(), h->type, sizeof(h->type));
    strcpy((char*)h->data, softver.toLatin1());

    return sizeof(struct UpdateHeader) + softver.count();
}
int UpdateApp::CodeDataPacket(const QByteArray &b, int seq, void *out, size_t maxlen)
{
    if (seq <= 0) return -1;

    seq--;
    if (b.count() < block_size*seq) return -1;

    size_t len = min(block_size, b.count() - block_size*seq);
    if (maxlen < len) return -1;

    memcpy(out, b.data()+block_size*seq, len);
    return len;
}

int UpdateApp::CodeUpdatePacket(const QByteArray &b, int seq, void *out, size_t maxlen)
{
    struct UpdatePacket *p = (struct UpdatePacket *)out;
    put_le_val(seq, p->seq, sizeof(p->seq));
    p->ack = 0x01;
    put_le_val(fileCrc, p->crc, sizeof(p->crc));

    int ret = 0;
    int idx = sizeof(struct UpdatePacket);
    if (seq == 0)
    {
        ret = CodeHeadPacket(b, p->data, maxlen-idx);
    }
    else
    {
        ret = CodeDataPacket(b, seq, p->data, maxlen-idx);
    }
    if (ret < 0) return -1;
    p->len = ret;
    idx += ret;
    return idx;
}
//---------------------------------------------------------------------------------------
void UpdateApp::UpdateUi()
{
    switch (updateState)
    {
    case UPDATE_STATE_RUNNING:
        ui->pushButtonStart->setEnabled(true);
        ui->pushButtonStart->setText("结束升级");
        ui->groupBoxUpdateFile->setEnabled(false);
        break;
    case UPDATE_STATE_STOPING:
        ui->pushButtonStart->setEnabled(false);
        break;
    case UPDATE_STATE_IDLE:
    default:
        ui->pushButtonStart->setEnabled(true);
        ui->pushButtonStart->setText("开始升级");
        ui->groupBoxUpdateFile->setEnabled(true);
        break;
    }
}
//---------------------------------------------------------------------------------------
int UpdateApp::GetNextRow(int from)
{
    int rows = ui->tableWidgetDevList->rowCount();

    for (int i = from; i < rows; i++)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(
                    ui->tableWidgetDevList->cellWidget(i, COL_AID));
        if (checkBox->isChecked())
        {
            return i;
        }
    }
    return -1;
}
int UpdateApp::GetUpdateBySeq(UpdateArg *arg, int seq, bool nextRow)
{
    uint8_t tmp[0x200];

    if (nextRow)
    {
        arg->curRow = GetNextRow(arg->curRow+1);
        if (arg->curRow < 0) return 1;
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(
                ui->tableWidgetDevList->cellWidget(updateArg->curRow, COL_AID));
        arg->devID = checkBox->text().toInt();
        resendCnt = 0;
        seq = 0;
    }

    int len = CodeUpdatePacket(updateData, seq, tmp, sizeof(tmp));
    if (len < 0) return -SMART_ENODATA;
    task->dlen = CodeUpdateFrame(setting->master_id, arg->devID,
                                 update_cmd[ui->comboBoxUpdateType->currentIndex()],
                                 tmp, len, task->data, sizeof(task->data));
    curSeq = seq;
    task->args = arg;
    task->taskInf = this;
    return task_man->AddTask(task);
}
//---------------------------------------------------------------------------------------
void UpdateApp::OnTaskDone(Task *task)
{
    bool nextRow = false;
    int err = task->dlen;
    struct UpdateArg *arg = (struct UpdateArg *)task->args;

    if (updateState == UPDATE_STATE_RUNNING)
    {
        QTableWidgetItem *item = ui->tableWidgetDevList->item(arg->curRow, COL_STATUS);
        if (task->dlen > 0)
        {
            struct SmartFrame *frame = get_smart_frame(task->data, task->dlen);
            struct UpdatePacket *u = (struct UpdatePacket *)&frame->data[1];

            int seq = get_le_val(u->seq, sizeof(u->seq));
            switch (seq)
            {
            case 0: item->setText("升级失败"); nextRow = true; break;
            case UPDATE_FINISHED: nextRow = true; break;
            default:resendCnt = 0;curSeq = seq; item->setText(QString("%1/%2").arg(curSeq).arg(blockNr));
                break;
            }
        }
        else if (task->dlen == -SMART_ETMOUT)
        {
            resendCnt++;
            nextRow = resendCnt < 3 ? false : true;
            if (nextRow) item->setText("升级失败");
        }
        err = GetUpdateBySeq(arg, curSeq, nextRow);
    }
    if (err == 0) return;

    updateState = UPDATE_STATE_IDLE;
    UpdateUi();
    QMessageBox::information(this, "信息", QString("升级完成!!!"));
}
//---------------------------------------------------------------------------------------
ComWidget *CreateUpdateApp(QWidget *parent, TaskMan *task_man)
{
    return new UpdateApp(task_man, parent);
}
//---------------------------------------------------------------------------------------
void UpdateApp::ClearTableWidget(void)
{
    int rows = ui->tableWidgetDevList->rowCount();

    while (rows--)
    {
        ui->tableWidgetDevList->removeRow(0);
    }
}
void UpdateApp::on_pushButtonImport_clicked()
{
    QSqlQuery q;

    QString sql(QObject::tr("SELECT AID FROM SmartDevices"));
    qDebug() << sql;
    if (!q.exec(sql)) {
        qDebug() << DDMError();
        return;
    }
    ClearTableWidget();

    int row = 0;
    while (q.next())
    {
        ui->tableWidgetDevList->insertRow(row);
        ui->tableWidgetDevList->setCellWidget(row, COL_AID,
                                              new QCheckBox(q.value(0).toString()));
        ui->tableWidgetDevList->setItem(row, COL_STATUS, new QTableWidgetItem("0/0"));
        row++;
    }
    on_pushButtonSelAll_clicked();
}
//---------------------------------------------------------------------------------------
void UpdateApp::on_pushButtonSelAll_clicked()
{
    int rows = ui->tableWidgetDevList->rowCount();

    while (rows--)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(
                    ui->tableWidgetDevList->cellWidget(rows, COL_AID));
        checkBox->setChecked(true);
    }
}

void UpdateApp::on_pushButtonDeSel_clicked()
{
    int rows = ui->tableWidgetDevList->rowCount();

    while (rows--)
    {
        QCheckBox *checkBox = dynamic_cast<QCheckBox*>(
                    ui->tableWidgetDevList->cellWidget(rows, COL_AID));
        checkBox->setChecked(!checkBox->isChecked());
    }
}
//---------------------------------------------------------------------------------------
void UpdateApp::on_pushButtonStart_clicked()
{
    assert(updateState != UPDATE_STATE_STOPING);

    /* user click stop button, we wait for the task's return */
    if (updateState == UPDATE_STATE_RUNNING)
    {
        updateState = UPDATE_STATE_STOPING;
        UpdateUi();
        return;
    }

    if (ui->lineEditPath->text().isEmpty())
    {
        QMessageBox::critical(this, "错误", "请选择升级文件");
        return;
    }

    if (ParseUpdateFileHeader(ui->lineEditPath->text()) < 0) return;

    updateArg->curRow = -1;
    int err = GetUpdateBySeq(updateArg, 0, true);
    if (err == 0)
    {
        updateState = UPDATE_STATE_RUNNING;
        UpdateUi();
        return;
    }

    if (err > 0)
    {
        QMessageBox::critical(this, "错误", "请选择升级设备");
    }
    else if (err < 0)
    {
        QMessageBox::critical(this, "错误", QString(SmartError(-err)));
    }
}
//---------------------------------------------------------------------------------------
