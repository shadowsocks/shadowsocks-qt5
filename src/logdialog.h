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
