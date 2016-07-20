#ifndef DEVSETWIDGET_H
#define DEVSETWIDGET_H

//---------------------------------------------------------------------------------------
#include "../CommApp/BaseApp.h"
#include "../TaskMan.h"
//---------------------------------------------------------------------------------------
namespace Ui {
class DevSet;
}
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
struct DevSetArg;
class QPushButton;
class QButtonGroup;
class GlobalSetting;
class DevSetWidget : public ComWidget, public TaskInf
{
    Q_OBJECT

public:
    explicit DevSetWidget(TaskMan *task_man, QWidget *parent = 0);
    ~DevSetWidget();

private slots:
    void OnActDevListTriggered();
    void OnActSelectAllTriggered();
    void OnActDeSelectTriggered();
    void OnActStartTriggered();

private:
    Ui::DevSet *ui;
    QButtonGroup *btnGroup;
    QPushButton *actDevList, *actSelectAll, *actDeSelect, *actStart;

    Task *task;
    TaskMan *task_man;
    GlobalSetting *setting;
    struct DevSetArg *devArg;

    int status;

    int DoSet(int row);
    int GetNextRow(int from);
    void UpdateUI(void);
    void SetupAction(void);
    void SetupBtnGroup(void);
    void ClearTableWidget(void);
    void ClearTableStatus(void);

    virtual void OnTaskDone(Task *task);
};

//---------------------------------------------------------------------------------------
extern "C"
{
    ComWidget *CreateDevSetApp(QWidget *parent, TaskMan *task_man);
}
//---------------------------------------------------------------------------------------

#endif // DEVSET_H
