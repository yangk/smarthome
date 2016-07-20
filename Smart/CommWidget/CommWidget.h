#ifndef HEXEDITDELEGATE_H
#define HEXEDITDELEGATE_H

//---------------------------------------------------------------------------------------
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QItemDelegate>
//---------------------------------------------------------------------------------------
class QLineEdit;
class MySpinBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit MySpinBox(const QString &title, uint32_t val, QWidget *parent = 0);

    uint32_t GetVal(void);
    void SetVal(uint32_t val);

private slots:
    void OnEdtDecEditingFinished();
    void OnEdtHexEditingFinished();

private:
    QLineEdit *edtDec, *edtHex;
};
//---------------------------------------------------------------------------------------
class QLabel;
class QLineEdit;
class LineEditWithLabel : public QWidget
{
    Q_OBJECT

public:
    explicit LineEditWithLabel(const QString &lbl_text, const QString &text, QWidget *parent = 0);

private:
    QLabel *lbl;
    QLineEdit *edt;
};

class LineEditWithAction : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEditWithAction(const QString &text, QWidget *parent = 0);

signals:
    void clicked();
};

class GroupBoxWithLineEdit : public QGroupBox
{
    Q_OBJECT

public:
    explicit GroupBoxWithLineEdit(const QString &title, const QString &text, QWidget *parent = 0);

    QString text(void) const;
    void settext(const QString &text);

private:
    QLineEdit *edt;
};

//---------------------------------------------------------------------------------------
class HexEditDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    HexEditDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
};
class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ComboBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
};
//---------------------------------------------------------------------------------------
#endif // HEXEDITDELEGATE_H
