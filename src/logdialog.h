/*
 * LogDialog Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = 0);
    ~LogDialog();

    void clear();

public slots:
    void append(const QString &);

private:
    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
