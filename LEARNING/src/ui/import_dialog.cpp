#include "import_dialog.h"

#include "../core/import_service.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

ImportDialog::ImportDialog(ImportService *service, QWidget *parent)
    : QDialog(parent),
      m_importService(service),
      m_previewList(nullptr)
{
    setWindowTitle(tr("导入记录"));
    setModal(true);
    buildUi();
}

QVector<Record> ImportDialog::importedRecords() const
{
    return m_records;
}

void ImportDialog::browse()
{
    const QString filePath = QFileDialog::getOpenFileName(this, tr("选择导入文件"), QString(), tr("文本文件 (*.txt);;所有文件 (*.*)"));
    if (filePath.isEmpty())
    {
        return;
    }

    QString errorMessage;
    const QVector<Record> records = m_importService->parseTextFile(filePath, &errorMessage);
    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("导入失败"), errorMessage);
        return;
    }

    preview(records);
}

void ImportDialog::preview(const QVector<Record> &records)
{
    m_records = records;
    m_previewList->clear();
    for (const Record &record : records)
    {
        const QString summary = tr("%1 (%2)").arg(record.title, record.username);
        m_previewList->addItem(summary);
    }
}

void ImportDialog::buildUi()
{
    auto *layout = new QVBoxLayout(this);

    auto *instruction = new QLabel(tr("导入文件需按行排列：标题、账号、密码、分类（可选）。空行分隔每条记录，""//""开头的行会被忽略。"), this);
    instruction->setWordWrap(true);
    layout->addWidget(instruction);

    auto *previewLabel = new QLabel(tr("预览"), this);
    layout->addWidget(previewLabel);

    m_previewList = new QListWidget(this);
    layout->addWidget(m_previewList);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons->button(QDialogButtonBox::Open), &QPushButton::clicked, this, &ImportDialog::browse);
    connect(buttons, &QDialogButtonBox::accepted, this, &ImportDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ImportDialog::reject);
    layout->addWidget(buttons);
}
