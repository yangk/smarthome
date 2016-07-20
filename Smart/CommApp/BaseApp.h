#ifndef COMWIDGET_H
#define COMWIDGET_H

#include <QWidget>
#include "Database/ddm.h"

class ComWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComWidget(QWidget *parent = 0);
    virtual ~ComWidget() = 0;

    virtual void OnTestCaseChanged(QList<TestCaseInfo> *testCaseInfoList)
    {
        Q_UNUSED(testCaseInfoList);
    }

signals:

public slots:

private:

};

#endif // COMWIDGET_H
