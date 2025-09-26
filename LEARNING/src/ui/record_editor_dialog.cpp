#include "record_editor_dialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

RecordEditorDialog::RecordEditorDialog(QWidget *parent)
    : QDialog(parent),
      m_titleEdit(nullptr),
      m_usernameEdit(nullptr),
      m_passwordEdit(nullptr),
      m_categoryEdit(nullptr),
      m_notesEdit(nullptr)
{
    setWindowTitle(tr("编辑记录"));
    setModal(true);
    buildUi();
}

void RecordEditorDialog::setRecord(const Record &record)
{
    m_titleEdit->setText(record.title);
    m_usernameEdit->setText(record.username);
    m_passwordEdit->setText(record.password);
    m_categoryEdit->setText(record.category);
    m_notesEdit->setPlainText(record.notes);
}

Record RecordEditorDialog::record() const
{
    Record record;
    record.title = m_titleEdit->text().trimmed();
    record.username = m_usernameEdit->text().trimmed();
    record.password = m_passwordEdit->text();
    record.category = m_categoryEdit->text().trimmed();
    record.notes = m_notesEdit->toPlainText().trimmed();
    return record;
}

void RecordEditorDialog::buildUi()
{
    m_titleEdit = new QLineEdit(this);
    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_categoryEdit = new QLineEdit(this);
    m_notesEdit = new QTextEdit(this);

    auto *formLayout = new QFormLayout;
    formLayout->addRow(tr("标题"), m_titleEdit);
    formLayout->addRow(tr("账号"), m_usernameEdit);
    formLayout->addRow(tr("密码"), m_passwordEdit);
    formLayout->addRow(tr("分类"), m_categoryEdit);
    formLayout->addRow(tr("备注"), m_notesEdit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &RecordEditorDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &RecordEditorDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addWidget(buttons);
}
