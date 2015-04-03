/*
 * EditDialog Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "connection.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(Connection *_connection, QWidget *parent = 0);
    ~EditDialog();

private:
    Ui::EditDialog *ui;
    Connection *connection;

private slots:
    void save();
};

#endif // EDITDIALOG_H
