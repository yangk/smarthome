#ifndef TASKMAN_H
#define TASKMAN_H

//---------------------------------------------------------------------------------------
#include <QObject>
#include "SmartFrame.h"
#include "Comportdialog.h"

//---------------------------------------------------------------------------------------
class Task;
class TaskInf
{
public:
    virtual void OnTaskDone(Task *task) = 0;
};

class Task
{
public:
    Task(){taskInf = NULL;}

    int dlen;
    uint8_t data[0x200];
    void *args;
    TaskInf *taskInf;
};
//---------------------------------------------------------------------------------------
class QTimer;
class ComportDialog;
class TaskMan : public QObject, public ComInf
{
    Q_OBJECT
public:
    ComportDialog *com;

    explicit TaskMan(ComportDialog *com, QObject *parent = 0);

    int AddTask(const Task *task);
    void SetOverTime(int ms);

signals:

public slots:
    void OnTimeout(void);

private:
    QTimer *tmr;

    const Task *task;
    int overtime;

    void OnTaskDone(const void *data, int len);

    int OnRcvData(const void *data, int len);
    virtual int OnSerialData(const void *data, int len);
    virtual int OnTcpData(const void *data, int len);
    virtual int OnUdpData(const void *data, int len);
};
//---------------------------------------------------------------------------------------
#endif // TASKMAN_H
