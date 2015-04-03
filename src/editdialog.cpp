#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    connection(_connection)
{
    ui->setupUi(this);
}

EditDialog::~EditDialog()
{
    delete ui;
}
