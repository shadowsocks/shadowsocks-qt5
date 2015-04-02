#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::clear()
{
    ui->logBrowser->clear();
}

void LogDialog::append(const QString &str)
{
    ui->logBrowser->append(str);
    ui->logBrowser->moveCursor(QTextCursor::End);
}
