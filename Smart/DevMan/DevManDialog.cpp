#include <QtWidgets>
#include <QSqlTableModel>
#include <QSqlQuery>
#include "DevManDialog.h"
#include "ui_DevManDialog.h"

#define TABLE_NAME "SmartDevices"
//---------------------------------------------------------------------------------------
enum
{
    DBCOL_AID, DBCOL_PASSWD, DBCOL_NAME, DBCOL_PANID, DBCOL_GROUPNO, DBCOL_KEY,
};
//---------------------------------------------------------------------------------------
DevManDialog::DevManDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevManDialog)
{
    ui->setupUi(this);
    SetupTreeView();
}

DevManDialog::~DevManDialog()
{
    delete ui;
}
//---------------------------------------------------------------------------------------
void DevManDialog::SetupTreeView(void)
{
    model = new QSqlTableModel(this);

    model->setTable(TABLE_NAME);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(DBCOL_AID, Qt::Horizontal, QObject::tr("地址"));
    model->setHeaderData(DBCOL_PASSWD, Qt::Horizontal, QObject::tr("密码"));
    model->setHeaderData(DBCOL_NAME, Qt::Horizontal, QObject::tr("名称"));
    model->setHeaderData(DBCOL_PANID, Qt::Horizontal, QObject::tr("网络号"));
    model->setHeaderData(DBCOL_GROUPNO, Qt::Horizontal, QObject::tr("组号"));
    model->setHeaderData(DBCOL_KEY, Qt::Horizontal, QObject::tr("主干密钥"));
    model->select();

    ui->tableViewDev->setAlternatingRowColors(true);
    ui->tableViewDev->setModel(model);
   // ui->tableViewDev->setItemDelegate(new QSqlRelationalDelegate(ui->tableViewDev));
    ui->tableViewDev->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewDev->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableViewDev->setColumnHidden(0, true);
    ui->tableViewDev->resizeColumnsToContents();
    ui->tableViewDev->resizeRowsToContents();
    ui->tableViewDev->horizontalHeader()->setStretchLastSection(true);

    //ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    //ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");

    QPushButton *btn = ui->buttonBox->addButton(tr("增加"), QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnAddClicked()));

    btn = ui->buttonBox->addButton(tr("删除"), QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnDelClicked()));

    btn = ui->buttonBox->addButton(tr("提交"), QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnSubmitClicked()));

    btn = ui->buttonBox->addButton(tr("撤销"), QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnRevertClicked()));
}
//---------------------------------------------------------------------------------------
void DevManDialog::OnBtnAddClicked()
{
    model->insertRow(model->rowCount());
}
void DevManDialog::OnBtnDelClicked()
{
    if (QMessageBox::warning(this,tr("警告"),tr("你确定删除当前设备吗？"), QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    model->removeRow(ui->tableViewDev->currentIndex().row());
}
void DevManDialog::OnBtnSubmitClicked()
{
    if(QMessageBox::warning(this,tr("警告"),tr("你确定提交当前所有修改吗？"), QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    QString msg;
    model->database().transaction();
    if (model->submitAll())
    {
        model->database().commit();
        msg = tr("提交成功");
    }
    else
    {
        model->database().rollback();
        msg = tr("提交失败");
    }

    QMessageBox::information(this,tr("信息"),msg);
}
void DevManDialog::OnBtnRevertClicked()
{
    if(QMessageBox::warning(this,tr("警告"),tr("你确定撤销当前所有修改吗？"), QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    model->revertAll();
}
//---------------------------------------------------------------------------------------
