#ifndef TESTCASE_H
#define TESTCASE_H
//---------------------------------------------------------------------------------------
#include <stdint.h>
#include <QDialog>
#include <QList>
#include <QFileInfo>


//---------------------------------------------------------------------------------------
#define TEST_CASE_DIR   "./testcase"

//---------------------------------------------------------------------------------------
namespace Ui {
class TestCaseDialog;
}

//---------------------------------------------------------------------------------------
class QSqlTableModel;
class TestCaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestCaseDialog(QWidget *parent = 0);
    ~TestCaseDialog();

signals:
    void SigTestCaseChanged(void);

public slots:
    void OnBtnAddClicked();
    void OnBtnDelClicked();
    void OnBtnSubmitClicked();
    void OnBtnRevertClicked();
    void OnBtnRefreshClicked();

private:
    Ui::TestCaseDialog *ui;
    QSqlTableModel  *model;

    void CheckDataBaseTabel(void);
    void SetupTreeView(void);
};
//---------------------------------------------------------------------------------------
#endif // TESTCASE_H
