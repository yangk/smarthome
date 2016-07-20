#include <QDir>
#include <QtWidgets>
#include <QtSql>
#include "CommFunc.h"
#include "CommWidget/CommWidget.h"
#include "ui_TestCaseDialog.h"
#include "TestCase.h"

//---------------------------------------------------------------------------------------
#define TABLE_NAME "TestCases"
enum
{
    COL_ID, COL_DID, COL_NAME, COL_CMD, COL_DATA
};
//---------------------------------------------------------------------------------------
TestCaseDialog::TestCaseDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TestCaseDialog)
{
    ui->setupUi(this);
    CheckDataBaseTabel();
    SetupTreeView();
}
TestCaseDialog::~TestCaseDialog()
{
}
//---------------------------------------------------------------------------------------
void TestCaseDialog::CheckDataBaseTabel(void)
{
    QSqlQuery q;

    q.exec(QObject::tr("SELECT name FROM sqlite_master WHERE type='table'"));
    while (q.next())
    {
        if (q.value(0).toString() == QString(TABLE_NAME))
            return;
    }

    /* init the "TestCases" table */
    q.exec("DROP TABLE TestCases");
    q.exec("CREATE TABLE TestCases ("
           "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
           "DID CHAR(3), "
           "TIPS VARCHAR(40), "
           "CMD CHAR(2), "
           "DATA VARCHAR(100))");
}

void TestCaseDialog::SetupTreeView(void)
{
    model = new QSqlTableModel(this);

    model->setTable(TABLE_NAME);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(COL_ID, Qt::Horizontal, "ID");
    model->setHeaderData(COL_DID, Qt::Horizontal, "数据标识");
    model->setHeaderData(COL_NAME, Qt::Horizontal, "描述");
    model->setHeaderData(COL_CMD, Qt::Horizontal, "命令字");
    model->setHeaderData(COL_DATA, Qt::Horizontal, "数据域");
    model->select();

    ui->tableViewTestCase->setAlternatingRowColors(true);
    ui->tableViewTestCase->setModel(model);
    //ui->tableViewTestCase->setItemDelegateForColumn(0, new HexEditDelegate(ui->tableViewTestCase));
    ui->tableViewTestCase->setItemDelegateForColumn(COL_CMD,
                                                    new ComboBoxDelegate(ui->tableViewTestCase));

    ui->tableViewTestCase->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewTestCase->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewTestCase->setColumnHidden(0, true);
    ui->tableViewTestCase->resizeColumnsToContents();
    ui->tableViewTestCase->resizeRowsToContents();
    ui->tableViewTestCase->horizontalHeader()->setStretchLastSection(true);

    //ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");

    QPushButton *btn = ui->buttonBox->addButton("增加", QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnAddClicked()));

    btn = ui->buttonBox->addButton("删除", QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnDelClicked()));

    btn = ui->buttonBox->addButton("提交", QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnSubmitClicked()));

    btn = ui->buttonBox->addButton("撤销", QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnRevertClicked()));

    btn = ui->buttonBox->addButton("刷新", QDialogButtonBox::ActionRole);
    connect(btn, SIGNAL(clicked(bool)), SLOT(OnBtnRefreshClicked()));
}
//---------------------------------------------------------------------------------------
void TestCaseDialog::OnBtnAddClicked()
{
    model->insertRow(model->rowCount());
}
void TestCaseDialog::OnBtnDelClicked()
{
    if (QMessageBox::warning(this,tr("警告"),tr("你确定删除当前设备吗？"),
                             QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    model->removeRow(ui->tableViewTestCase->currentIndex().row());
}
void TestCaseDialog::OnBtnSubmitClicked()
{
    if(QMessageBox::warning(this,tr("警告"),tr("你确定提交当前所有修改吗？"),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    QString msg;
    model->database().transaction();
    if (model->submitAll())
    {
        model->database().commit();
        msg = tr("提交成功");
        emit SigTestCaseChanged();
    }
    else
    {
        model->database().rollback();
        msg = tr("提交失败");
    }

    QMessageBox::information(this,tr("信息"),msg);
}
void TestCaseDialog::OnBtnRevertClicked()
{
    if(QMessageBox::warning(this,tr("警告"),tr("你确定撤销当前所有修改吗？"),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
        return;

    model->revertAll();
}
void TestCaseDialog::OnBtnRefreshClicked()
{
    model->select();
}

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
