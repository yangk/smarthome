#include <QTimer>
#include <assert.h>
#include "TaskMan.h"
#include "errorcode.h"
#include "Comportdialog.h"

//---------------------------------------------------------------------------------------
TaskMan::TaskMan(ComportDialog *com, QObject *parent) : QObject(parent), com(com)
{
    task = NULL;
    overtime = 2000;

    tmr = new QTimer(this);
    tmr->setSingleShot(true);
    connect(tmr, SIGNAL(timeout()), SLOT(OnTimeout()));
}

void TaskMan::SetOverTime(int ms)
{
    overtime = ms;
}
//---------------------------------------------------------------------------------------
int TaskMan::AddTask(const Task *task)
{
    if (!com->IsCommOpen()) return -SMART_ESERIAL_NOOPEN;
    if (this->task) return -SMART_EBUSY;
    if (task->dlen <= 0) return -SMART_ENODATA;

    SmartFrame *pframe = get_smart_frame(task->data, task->dlen);
    if (!pframe) return -SMART_ENODATA;

    if (!is_broad_frame(pframe))
    {
        this->task = task;
        tmr->start(overtime);
    }

    com->write(ComportDialog::ComChnSerial, task->data, task->dlen);
    return SMART_ENO;
}
//---------------------------------------------------------------------------------------
void TaskMan::OnTaskDone(const void *data, int len)
{
    if (tmr->isActive())
        tmr->stop();

    if (task)
    {
        Task *ptask = (Task *)task;
        this->task = NULL;

        ptask->dlen = len;
        if (ptask->dlen > 0)
            memcpy(ptask->data, data, ptask->dlen);

        if (ptask->taskInf)
            ptask->taskInf->OnTaskDone(ptask);
    }
}
//---------------------------------------------------------------------------------------
void TaskMan::OnTimeout(void)
{
    assert(task);
    OnTaskDone(NULL, -SMART_ETMOUT);
}
//---------------------------------------------------------------------------------------
int TaskMan::OnRcvData(const void *data, int len)
{
    SmartFrame *frame = get_smart_frame((const uint8_t*)data, len);
    if (!frame) return 0;

    if (is_remote_frame(frame) && !is_reply_frame(frame))
        return len;

    OnTaskDone(frame, frame_len(frame));
    return len;
}

int TaskMan::OnSerialData(const void *data, int len)
{
    return OnRcvData(data, len);
}
int TaskMan::OnTcpData(const void *data, int len)
{
    return OnRcvData(data, len);
}
int TaskMan::OnUdpData(const void *data, int len)
{
    return OnRcvData(data, len);
}
//---------------------------------------------------------------------------------------
