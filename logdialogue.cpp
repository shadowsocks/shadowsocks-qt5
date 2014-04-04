#include "logdialogue.h"
#include "ui_logdialogue.h"

LogDialogue::LogDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialogue)
{
    ui->setupUi(this);
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
