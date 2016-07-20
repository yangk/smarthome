#ifndef MODEMSETTING_H
#define MODEMSETTING_H

//---------------------------------------------------------------------------------------
#include <QDialog>
#include "TaskMan.h"
//---------------------------------------------------------------------------------------
namespace Ui {
class ModemSetDialog;
}
//---------------------------------------------------------------------------------------
class GlobalSetting;
class ModemSetting : public QDialog, public TaskInf
{
    Q_OBJECT

public:
    explicit ModemSetting(TaskMan *task_man, QWidget *parent = 0);
    ~ModemSetting();

private slots:
    void slot_accept(void);
    void OnBtnClicked();

private:
    Ui::ModemSetDialog *ui;
    GlobalSetting *setting;

    Task *task;
    TaskMan *task_man;

    virtual void OnTaskDone(Task *task);
};
//---------------------------------------------------------------------------------------
#endif // MODEMSETTING_H
