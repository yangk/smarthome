#include "ToolAscStr.h"
#include "ui_ToolAscStr.h"
#include "CommFunc.h"
//---------------------------------------------------------------------------------------
ToolAscStrDialog::ToolAscStrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolAscStr)
{
    ui->setupUi(this);
}

ToolAscStrDialog::~ToolAscStrDialog()
{
    delete ui;
}
//---------------------------------------------------------------------------------------
void ToolAscStrDialog::on_lineEditTextStr_editingFinished()
{
    char buf[0x100], asc[0x100];

    strncpy(asc, ui->lineEditTextStr->text().toLatin1(), sizeof(asc));

    ui->lineEditTextHex->setText(arr2str(asc, strlen(asc), buf, sizeof(buf)));
    ui->labelStatus->setText("转换完成");
}
void ToolAscStrDialog::on_lineEditTextHex_editingFinished()
{
    char buf[0x100], asc[0x100];

    strncpy(asc, ui->lineEditTextHex->text().toLatin1(), sizeof(asc));

    if (!is_good_xdstr(asc))
    {
        ui->labelStatus->setText("十六进制格式错误!");
        return;
    }
    int ret = str2arr(asc, buf, sizeof(buf));
    buf[ret] = '\0';
    ui->lineEditTextStr->setText(buf);
    ui->labelStatus->setText("转换完成");
}
void ToolAscStrDialog::on_checkBoxReverse_toggled(bool checked)
{
    Q_UNUSED(checked);
    char asc[0x100];

    strncpy(asc, ui->lineEditTextStr->text().toLatin1(), sizeof(asc));
    reverse(asc, strlen(asc));
    ui->lineEditTextStr->setText(asc);
}
//---------------------------------------------------------------------------------------
void ToolAscStrDialog::on_lineEditNumHex_editingFinished()
{
    char buf[0x100], asc[0x100];
    uint64_t (*get_val)(const uint8_t *, size_t);

    strncpy(asc, ui->lineEditNumHex->text().toLatin1(), sizeof(asc));

    if (!is_good_xdstr(asc))
    {
        ui->labelStatus->setText("十六进制格式错误!");
        return;
    }
    int ret = str2arr(asc, buf, sizeof(buf));
    get_val = ui->checkBoxBigEndian->isChecked() ? get_be_val : get_le_val;
    uint64_t val = get_val((uint8_t*)buf, ret);

    ui->lineEditNumDec->setText(QString::asprintf("%llu", val));
    ui->labelStatus->setText("转换完成");
}
void ToolAscStrDialog::on_lineEditNumDec_editingFinished()
{
    char buf[0x100], asc[0x100];
    void (*put_val)(uint64_t, uint8_t *, size_t);

    char *rest = NULL;
    strncpy(asc, ui->lineEditNumDec->text().toLatin1(), sizeof(asc));
    uint64_t val = strtoull(asc, &rest, 10);
    if (*rest)
    {
        ui->labelStatus->setText("十进制格式错误!");
        return;
    }

    put_val = ui->checkBoxBigEndian->isChecked() ? put_be_val : put_le_val;
    put_val(val, (uint8_t*)buf, sizeof(uint64_t));

    ui->lineEditNumHex->setText(arr2str(buf, sizeof(uint64_t), asc, sizeof(asc)));
    ui->labelStatus->setText("转换完成");
}
//---------------------------------------------------------------------------------------
