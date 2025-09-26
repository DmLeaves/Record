#pragma once

#include "../core/record.h"

#include <QDialog>

class QLineEdit;
class QTextEdit;

class RecordEditorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecordEditorDialog(QWidget *parent = nullptr);

    void setRecord(const Record &record);
    Record record() const;

private:
    void buildUi();

    QLineEdit *m_titleEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_categoryEdit;
    QTextEdit *m_notesEdit;
};
