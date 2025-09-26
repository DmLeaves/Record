#pragma once

#include "../core/record.h"

#include <QDialog>
#include <QVector>

class ImportService;
class QListWidget;

class ImportDialog : public QDialog
{
    Q_OBJECT
public:
    ImportDialog(ImportService *service, QWidget *parent = nullptr);

    QVector<Record> importedRecords() const;

private slots:
    void browse();
    void preview(const QVector<Record> &records);

private:
    void buildUi();

    ImportService *m_importService;
    QVector<Record> m_records;
    QListWidget *m_previewList;
};
