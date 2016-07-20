#ifndef TOOLASCSTR_H
#define TOOLASCSTR_H

//---------------------------------------------------------------------------------------
#include <QDialog>
//---------------------------------------------------------------------------------------
namespace Ui {
class ToolAscStr;
}
//---------------------------------------------------------------------------------------
class ToolAscStrDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolAscStrDialog(QWidget *parent = 0);
    ~ToolAscStrDialog();

private slots:
    void on_lineEditTextStr_editingFinished();
    void on_lineEditTextHex_editingFinished();
    void on_lineEditNumHex_editingFinished();
    void on_lineEditNumDec_editingFinished();
    void on_checkBoxReverse_toggled(bool checked);

private:
    Ui::ToolAscStr *ui;
};
//---------------------------------------------------------------------------------------
#endif // TOOLASCSTR_H
