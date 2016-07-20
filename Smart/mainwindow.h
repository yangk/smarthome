#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdint.h>
#include <QMainWindow>
#include "Comportdialog.h"
#include "LogMan.h"
#include "Database/ddm.h"

namespace Ui {
class MainWindow;
}

struct FrameInfo
{
    int dir;
    uint8_t sAddr[4];
    uint8_t tAddr[4];
    int ctrl;
    uint8_t data[0x100];
};

class HLed;
class TaskMan;
class ComportDialog;
class ModemSetting;
class About;
class QSettings;
class QLabel;
class QTimer;
class QLineEdit;
class QComboBox;
class TestCaseDialog;
class QTreeWidgetItem;
class MainWindow : public QMainWindow, public FrameLogInf
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void AddFrameInfo(const struct FrameInfo *info);

private slots:
    void OnActCommportTriggered();
    void OnActModemTriggered();
    void OnActDevManTriggered();
    void OnActAboutTriggered();
    void OnActAscTriggered();
    void OnActTestCaseTriggered();
    void OnTimerUpdate();
    void slotCommOpen(bool open, QString&);
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void OnSaveToFile(void);
    void OnTestCaseInfoChange(void);
    void OnClearTableWidget(void);

    void on_tableWidgetFrame_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    ComportDialog *com;
    ModemSetting *modem_setting;
    About *about;
    QTimer *timer;
    QLabel *statusLblComm;
    QLabel *statusLblCurTime;
    QLabel *tipsLabel;

    QAction *actComSet, *actModem, *actDevMan, *actQuit, *actAbout;
    QAction *actAscTool, *actTestCase;

    HLed *led;
    TaskMan *task_man;
    QString now;

    TestCaseDialog *testCaseDialog;
    QList<TestCaseInfo> testCaseInfoList;

    void SetupAction();
    void SetupStatusBar();
    void SetupTimer();
    void setupFunctionList();
    void SetupRunEnv();
    void setupTextBrowser();

    virtual void FrameLog(FrameLogInf::DIR dir, const void *data, int len);
    virtual void Log(const QString &log);
    virtual void ShowStatus(const QString &status);
};

#endif // MAINWINDOW_H
