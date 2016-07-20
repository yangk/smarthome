#ifndef UPDATEAPP_H
#define UPDATEAPP_H

#include <QWidget>
#include "BaseApp.h"
#include "TaskMan.h"

//---------------------------------------------------------------------------------------
namespace Ui {
class UpdateWidget;
}
//---------------------------------------------------------------------------------------
class GlobalSetting;

#pragma pack(1)
struct UpdatePacket
{
    uint8_t seq[2];
    uint8_t ack;
    uint8_t crc[2];
    uint8_t len;
    uint8_t data[0];
};
struct UpdateHeader
{
    uint8_t size[4];
    uint8_t crc[2];
    uint8_t block_size;
    uint8_t type[8];
    uint8_t data[0];
};
#pragma pack()

enum UPDATE_STATE
{
    UPDATE_STATE_IDLE,
    UPDATE_STATE_RUNNING,
    UPDATE_STATE_STOPING,
};

class Task;
struct UpdateArg;
class UpdateApp : public ComWidget, public TaskInf
{
    Q_OBJECT

public:
    explicit UpdateApp(TaskMan *task_man, QWidget *parent = 0);
    ~UpdateApp();

private slots:
    void on_pushButtonGetFile_clicked();
    void on_pushButtonImport_clicked();
    void on_pushButtonSelAll_clicked();
    void on_pushButtonDeSel_clicked();
    void on_pushButtonStart_clicked();

private:
    Ui::UpdateWidget *ui;
    TaskMan *task_man;
    GlobalSetting *setting;

    int updateState, resendCnt;
    Task *task;
    int curSeq;
    int blockNr, block_size;
    uint16_t fileCrc;
    QByteArray updateData;

    struct UpdateArg *updateArg;


    //char *softver, *devtype, *crc, *size;
    QString softver, devtype, crc, size;
    virtual void OnTaskDone(Task *task);

    void UpdateUi();
    int GetNextRow(int from);
    int GetUpdateBySeq(UpdateArg *arg, int seq, bool nextRow);

    const char *GetInfoInHeader(const char *header, int hlen, const char *str);
    int ParseUpdateFileHeader(const QString &filename);

    void ClearTableWidget(void);

    int CodeHeadPacket(const QByteArray &b, void *out, size_t maxlen);
    int CodeDataPacket(const QByteArray &b, int seq, void *out, size_t maxlen);
    int CodeUpdatePacket(const QByteArray &b, int seq, void *out, size_t maxlen);
};

extern "C"
{
    ComWidget *CreateUpdateApp(QWidget *parent, TaskMan *task_man);
}
#endif // UPDATEAPP_H
