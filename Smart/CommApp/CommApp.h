#ifndef COMMAPP_H
#define COMMAPP_H
//---------------------------------------------------------------------------------------
#include <QList>
#include <QWidget>
#include "BaseApp.h"
//---------------------------------------------------------------------------------------
namespace Ui {
class CommAppWidget;
}
//---------------------------------------------------------------------------------------
class TaskMan;
class SendGroup;
class TestCaseDialog;
//---------------------------------------------------------------------------------------
class CommApp : public ComWidget
{
    Q_OBJECT

public:
    explicit CommApp(TaskMan *task_man, QWidget *parent = 0);
    ~CommApp();

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonDel_clicked();

private:
    Ui::CommAppWidget *ui;
    TestCaseDialog *testCaseDialog;

    TaskMan *task_man;
    QList<SendGroup *> sendGroups;
    QList<TestCaseInfo> *testCaseInfoList;

    void DoAddSendGroup();
    void DoDelSendGroup();

    virtual void OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList);
};

//---------------------------------------------------------------------------------------
extern "C"
{
    ComWidget *CreateCommApp(QWidget *parent, TaskMan *task_man);
}
//---------------------------------------------------------------------------------------
#endif // COMMAPP_H
