#include <QtWidgets>
#include "CommFunc.h"
#include "SuccessRatioApp.h"
#include "SendGroup.h"

//---------------------------------------------------------------------------------------
SuccessRatio::SuccessRatio(TaskMan *task_man, QWidget *parent) :
    ComWidget(parent), task_man(task_man)
{
    succeed_cnt = failed_cnt = 0;

    QVBoxLayout *lay = new QVBoxLayout(this);

    // 成功率显示
    lblRatio = new QLabel("0%", this);
    lblRatio->setAlignment(Qt::AlignCenter);
    lblRatio->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lblRatio->setStyleSheet("color:green; font-size:130px;");
    lay->addWidget(lblRatio);

    QHBoxLayout *layH = new QHBoxLayout();

    QGroupBox *g = new QGroupBox("统计计数", this);
    QGridLayout *layCal = new QGridLayout(g);
    lblSucceed = new QLabel("成功次数:0", g);
    lblSucceed->setStyleSheet("color:green;");
    lblFailed = new QLabel("失败次数:0", g);
    lblFailed->setStyleSheet("color:red;");
    lblAll = new QLabel("累计次数:0", g);
    layCal->addWidget(lblSucceed, 0, 0);
    layCal->addWidget(lblFailed, 0, 1);
    layCal->addWidget(lblAll, 1, 0);

    btn_resetcnt = new QPushButton("计数器复位", g);
    btn_resetcnt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(btn_resetcnt, SIGNAL(clicked(bool)), SLOT(OnBtnResetcntClicked(bool)));
    layCal->addWidget(btn_resetcnt, 1, 1);

    layH->addWidget(g);

    snd_group = new SendGroup(task_man, this);
    snd_group->SetTitle("组帧发送");
    connect(snd_group, SIGNAL(SigGetReply(bool)), SLOT(OnSndGroupReplyChanged(bool)));
    layH->addWidget(snd_group);

    lay->addLayout(layH);

    this->setLayout(lay);

    UpdateRatioDisplay();
}

SuccessRatio::~SuccessRatio()
{

}
//---------------------------------------------------------------------------------------
void SuccessRatio::UpdateRatioDisplay(void)
{
    float ratio = 100.0;
    if (succeed_cnt+failed_cnt != 0)
        ratio = succeed_cnt*100.0/(succeed_cnt+failed_cnt);

    ratio = min(ratio, 100.0);
    ratio = max(ratio, 0.0);

    lblRatio->setText(QString("%1%").arg(ratio, 0, 'f', 1));
    lblSucceed->setText(QString("成功次数:%1").arg(succeed_cnt));
    lblFailed->setText(QString("失败次数:%1").arg(failed_cnt));
    lblAll->setText(QString("累计次数:%1").arg(succeed_cnt+failed_cnt));
}
//---------------------------------------------------------------------------------------
void SuccessRatio::OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList)
{
    snd_group->OnTestCaseChanged(testCaseInfoList);
}
void SuccessRatio::OnSndGroupReplyChanged(bool succeed)
{
    if (succeed) succeed_cnt++;
    else failed_cnt++;

    UpdateRatioDisplay();
}
void SuccessRatio::OnBtnResetcntClicked(bool)
{
    succeed_cnt = failed_cnt = 0;

    UpdateRatioDisplay();
}
//---------------------------------------------------------------------------------------
ComWidget *CreateSuccessRatio(QWidget *parent, TaskMan *task_man)
{
    return new SuccessRatio(task_man, parent);
}
//---------------------------------------------------------------------------------------
