/*
 * LogDialog Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include "connection.h"

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(Connection *c, QWidget *parent = 0);
    ~LogDialog();

private:
    Ui::LogDialog *ui;
    Connection *con;

private slots:
    void append(const QString &);
    void onSaveClicked();
    void onClearClicked();
};

#endif // LOGDIALOG_H
