#ifndef DEVMANDIALOG_H
#define DEVMANDIALOG_H

#include <QDialog>

namespace Ui {
class DevManDialog;
}

class QSqlTableModel ;
class DevManDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DevManDialog(QWidget *parent = 0);
    ~DevManDialog();

private slots:
    void OnBtnAddClicked();
    void OnBtnDelClicked();
    void OnBtnSubmitClicked();
    void OnBtnRevertClicked();

private:
    Ui::DevManDialog *ui;
    QSqlTableModel  *model;

    void SetupTreeView(void);
};

#endif // DEVMANDIALOG_H
