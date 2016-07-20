#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QtCore/qglobal.h>

#if defined(ABOUT_LIBRARY)
#  define ABOUTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ABOUTSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Ui {
class Dialog;
}

class ABOUTSHARED_EXPORT About : public QDialog
{
public:
    About(QWidget *parent = 0);

private:
    Ui::Dialog *ui;

    QPushButton *btnClose;
};

#endif // ABOUT_H
