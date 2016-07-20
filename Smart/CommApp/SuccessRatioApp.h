#ifndef SUCCESSRATIO_H
#define SUCCESSRATIO_H
//---------------------------------------------------------------------------------------
#include "BaseApp.h"
//---------------------------------------------------------------------------------------
class QLabel;
class TaskMan;
class SendGroup;
class QPushButton;
class SuccessRatio : public ComWidget
{
    Q_OBJECT

public:
    explicit SuccessRatio(TaskMan *task_man, QWidget *parent = 0);
    ~SuccessRatio();

private slots:
    void OnSndGroupReplyChanged(bool succeed);
    void OnBtnResetcntClicked(bool);

private:
    TaskMan *task_man;
    SendGroup *snd_group;
    QLabel *lblRatio, *lblSucceed, *lblFailed, *lblAll;
    QPushButton *btn_resetcnt;

    uint32_t succeed_cnt, failed_cnt;
    void UpdateRatioDisplay(void);

    void OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList);
};
//---------------------------------------------------------------------------------------
extern "C"
{
    ComWidget *CreateSuccessRatio(QWidget *parent, TaskMan *task_man);
}
//---------------------------------------------------------------------------------------

#endif // SUCCESSRATIO_H
