#include <QtWidgets>
#include "CommFunc.h"
#include "CommWidget.h"


//---------------------------------------------------------------------------------------
MySpinBox::MySpinBox(const QString &title, uint32_t val, QWidget *parent)
    : QGroupBox(title, parent)
{
    QFormLayout *layMain = new QFormLayout(this);

    edtDec = new QLineEdit(this);
    edtDec->setInputMask("0000000000");
    layMain->addRow("Dec:", edtDec);

    edtHex = new QLineEdit(this);
    edtHex->setInputMask("HH HH HH HH");
    edtHex->setToolTip("小端模式");
    layMain->addRow("Hex:", edtHex);

    connect(edtDec, &QLineEdit::editingFinished, this, &MySpinBox::OnEdtDecEditingFinished);
    connect(edtHex, &QLineEdit::editingFinished, this, &MySpinBox::OnEdtHexEditingFinished);
    SetVal(val);
}

void MySpinBox::OnEdtDecEditingFinished()
{
    uint8_t arr[4];
    put_le_val(GetVal(), arr, sizeof(arr));

    char str[0x10];
    QString hex(arr2str(arr, sizeof(arr), str, sizeof(str)));
    if (edtHex->text() != hex) edtHex->setText(hex);
}

void MySpinBox::OnEdtHexEditingFinished()
{
    uint8_t arr[4];

    str2arr(edtHex->text().toLatin1(), arr, sizeof(arr));
    QString num = QString::number(get_le_val(arr, sizeof(arr)));
    if (edtDec->text() != (num)) edtDec->setText(num);
}

uint32_t MySpinBox::GetVal(void)
{
    return edtDec->text().toUInt();
}

void MySpinBox::SetVal(uint32_t val)
{
    edtDec->setText(QString::number(val));
    OnEdtDecEditingFinished();
}
//---------------------------------------------------------------------------------------
LineEditWithLabel::LineEditWithLabel(const QString &lbl_text, const QString &text, QWidget *parent)
    : QWidget(parent), lbl(new QLabel(lbl_text, this)), edt(new QLineEdit(text, this))
{
    QHBoxLayout *lay_main = new QHBoxLayout(this);
    lbl->setBuddy(this);
    lay_main->addWidget(lbl);
    lay_main->addWidget(edt);
}

LineEditWithAction::LineEditWithAction(const QString &text, QWidget *parent)
    : QLineEdit(text, parent)
{
    QAction *action = this->addAction(QIcon(":/File/images/open.ico"),
                                      QLineEdit::ActionPosition::TrailingPosition);
    connect(action, &QAction::triggered, this, &LineEditWithAction::clicked);
}


GroupBoxWithLineEdit::GroupBoxWithLineEdit(const QString &title, const QString &text, QWidget *parent)
    : QGroupBox(title, parent)
{
    QVBoxLayout *lay_main = new QVBoxLayout(this);
    edt = new QLineEdit(this);
    lay_main->addWidget(edt);
    edt->setText(text);
}

void GroupBoxWithLineEdit::settext(const QString &text)
{
    edt->setText(text);
}
QString GroupBoxWithLineEdit::text(void) const
{
    return edt->text();
}

//---------------------------------------------------------------------------------------
HexEditDelegate::HexEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}
QWidget *HexEditDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    MySpinBox *editor = new MySpinBox("", 0, parent);
    editor->installEventFilter(const_cast<HexEditDelegate*>(this));

    return editor;
}
void HexEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    MySpinBox *box = static_cast<MySpinBox*>(editor);
    box->SetVal(index.model()->data(index).toInt());
}
void HexEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    MySpinBox *box = static_cast<MySpinBox*>(editor);
    model->setData(index, box->GetVal());
}
void HexEditDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option); Q_UNUSED(index);
    editor->setStyleSheet("background-color: #f1f1f1");
//    editor->setGeometry(option.rect);
    editor->setGeometry(option.rect.left(), option.rect.top(),
                        editor->sizeHint().width(), editor->sizeHint().height());
}
//---------------------------------------------------------------------------------------
ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}
QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->addItems(QStringList() << "读" << "写");
    editor->installEventFilter(const_cast<ComboBoxDelegate*>(this));

    return editor;
}
void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    box->setCurrentText(index.model()->data(index).toString());
}
void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    model->setData(index, box->currentText());
}
void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(option); Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
//---------------------------------------------------------------------------------------
