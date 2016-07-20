#include <QLabel>
#include <QPushButton>
#include "about.h"
#include "ui_dialog.h"

About::About(QWidget *parent) :
    QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("关于");

    btnClose = new QPushButton("确定", this);
    connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));
    ui->gridLayout->addWidget(btnClose, 3, 3, 1, 1);

    ui->labelName->setText(QApplication::applicationName());
    ui->labelVer->setText(QApplication::applicationVersion());
}
