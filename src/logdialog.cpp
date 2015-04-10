#include "logdialog.h"
#include "ui_logdialog.h"
#include <QFileDialog>

LogDialog::LogDialog(Connection *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog),
    con(c)
{
    ui->setupUi(this);
    ui->logBrowser->setText(con->log);
    ui->logBrowser->moveCursor(QTextCursor::End);

    connect(con, &Connection::newLogAvailable, this, &LogDialog::append);
    connect(ui->clearButton, &QPushButton::clicked, this, &LogDialog::onClearClicked);
    connect(ui->saveAsButton, &QPushButton::clicked, this, &LogDialog::onSaveClicked);

    this->adjustSize();
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::append(const QString &str)
{
    ui->logBrowser->append(str);
    ui->logBrowser->moveCursor(QTextCursor::End);
}

void LogDialog::onSaveClicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Log As"));
    if (!file.isEmpty()) {
        QString log = ui->logBrowser->toPlainText();
        QFile logFile(file);
        logFile.open(QIODevice::WriteOnly);
        logFile.write(log.toLocal8Bit());
        logFile.close();
    }
}

void LogDialog::onClearClicked()
{
    ui->logBrowser->clear();
    con->log.clear();
}
