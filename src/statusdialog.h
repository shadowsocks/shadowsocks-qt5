#ifndef STATUSDIALOG_H
#define STATUSDIALOG_H

#include <QDialog>
#include "connection.h"

namespace Ui {
class StatusDialog;
}

class StatusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatusDialog(Connection*, QWidget *parent = 0);
    ~StatusDialog();

private:
    Ui::StatusDialog *ui;
    Connection *con;

    static QString convertToHumanReadable(quint64 bytes);
    static const QStringList units;

private slots:
    void onStatusChanged(bool);
    void onBytesReadChanged(const quint64 &);
    void onBytesSentChanged(const quint64 &);
};

#endif // STATUSDIALOG_H
