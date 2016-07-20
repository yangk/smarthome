#ifndef SENDGROUP_H
#define SENDGROUP_H
//---------------------------------------------------------------------------------------
#include <QGroupBox>
#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>
#include "TaskMan.h"
#include "Database/ddm.h"

//---------------------------------------------------------------------------------------
namespace Ui {
class SendGroupWidget;
}
//---------------------------------------------------------------------------------------
class GlobalSetting;
class SendGroup : public QWidget, public TaskInf
{
    Q_OBJECT

public:
    explicit SendGroup(TaskMan *taskMan, QWidget *parent = 0);
    ~SendGroup();

    void SetTitle(const QString &title);
    void OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList);

signals:
    void SigGetReply(bool);

private slots:
    void OnBtnSendClicked();
    void OnTimeout(void);

    void on_checkBoxRaw_toggled(bool checked);
    void on_checkBoxCycle_toggled(bool checked);
    void on_comboBoxTips_currentIndexChanged(int index);

    void on_pushButtonSave_clicked();

private:
    Ui::SendGroupWidget *ui;

    QTimer *timer;

    bool cycleSending;
    Task *task;
    TaskMan *taskMan;
    GlobalSetting *setting;
    QList<TestCaseInfo> *testCaseInfoList;

    int DoTask(void);
    int GetFrameFromUi(uint8_t *out, int maxlen);
    int GetFrameFromRawUi(uint8_t *out, int maxlen);

    virtual void OnTaskDone(Task *task);
};

#endif // SENDGROUP_H
