#include "logdialogue.h"
#include "ui_logdialogue.h"

LogDialogue::LogDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialogue)
{
    ui->setupUi(this);

#ifdef _WIN32
    ui->text->append(QString("Because of the buffer, log prints slower than it occurs."));
    ui->text->append(QString("---------------------------------------------------------"));
#endif
}

LogDialogue::~LogDialogue()
{
    delete ui;
}

void LogDialogue::onreadReadyProcess(const QByteArray &o)
{
    ui->text->moveCursor(QTextCursor::End);
    ui->text->append(o);
    ui->text->moveCursor(QTextCursor::End);
}
