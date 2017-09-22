#include "logdialog.h"
#include "ui_logdialog.h"
#include <QFileDialog>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    ui->logBrowser->moveCursor(QTextCursor::End);

    connect(ui->clearButton, &QPushButton::clicked, this, &LogDialog::onClearClicked);
    connect(ui->saveAsButton, &QPushButton::clicked, this, &LogDialog::onSaveClicked);

    this->adjustSize();
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::onSaveClicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Log As"));
    if (!file.isEmpty()) {
        QString log = ui->logBrowser->toPlainText();
        QFile logFile(file);
        logFile.open(QIODevice::WriteOnly);
        logFile.write(log.toUtf8());
        logFile.close();
    }
}

void LogDialog::onClearClicked()
{
    ui->logBrowser->clear();
}
