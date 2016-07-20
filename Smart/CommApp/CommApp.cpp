#include <QtCore>
#include <QtWidgets>
#include "Commfunc.h"
#include "../SmartFrame.h"
#include "../errorcode.h"
#include "../GlobalSetting.h"
#include "../TaskMan.h"
#include "SendGroup.h"
#include "mainwindow.h"
#include "CommApp.h"
#include "ui_CommAppWidget.h"

#define SEND_GROUP_NR   4

//---------------------------------------------------------------------------------------
CommApp::CommApp(TaskMan *task_man, QWidget *parent) :
    ComWidget(parent), ui(new Ui::CommAppWidget), task_man(task_man)
{
    ui->setupUi(this);

    testCaseInfoList = NULL;

    DoAddSendGroup();
    DoAddSendGroup();
}

CommApp::~CommApp()
{
}
//---------------------------------------------------------------------------------------
void CommApp::DoAddSendGroup()
{
    if (sendGroups.count() == SEND_GROUP_NR)
    {
        QMessageBox::critical(this, "错误", "已经达到最大限制");
        return;
    }

    SendGroup *g = new SendGroup(task_man, this);
    g->SetTitle(QString("自组帧%1").arg(sendGroups.count()));
    if (testCaseInfoList) g->OnTestCaseChanged(testCaseInfoList);
    ui->verticalLayout->insertWidget(sendGroups.count(), g);
    sendGroups.append(g);
}
void CommApp::DoDelSendGroup()
{
    if (sendGroups.count() == 1)
    {
        QMessageBox::critical(this, "错误", "不能删除最后一个");
        return;
    }

    SendGroup *g = sendGroups.takeLast();

    ui->verticalLayout->removeWidget(g);
    delete g;
}

void CommApp::on_pushButtonAdd_clicked()
{
    DoAddSendGroup();
}

void CommApp::on_pushButtonDel_clicked()
{
    DoDelSendGroup();
}
//---------------------------------------------------------------------------------------
void CommApp::OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList)
{
    this->testCaseInfoList = testCaseInfoList;
    foreach (SendGroup *g, sendGroups)
    {
        g->OnTestCaseChanged(testCaseInfoList);
    }
}

//---------------------------------------------------------------------------------------
ComWidget *CreateCommApp(QWidget *parent, TaskMan *task_man)
{
    return new CommApp(task_man, parent);
}
//---------------------------------------------------------------------------------------
