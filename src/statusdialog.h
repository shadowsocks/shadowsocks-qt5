/*
 * Copyright (C) 2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * shadowsocks-qt5 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-qt5 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

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
    void onBytesChanged(const quint64 &current, const quint64 &total);
};

#endif // STATUSDIALOG_H
