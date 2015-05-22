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

#ifndef CONTROLLERTHREAD_H
#define CONTROLLERTHREAD_H

#include <QThread>
#include <QtShadowsocks>

class ControllerThread : public QThread
{
    Q_OBJECT
public:
    ControllerThread(QObject *parent);
    void setup(bool _debug, const QSS::Profile &_profile);
    void run() Q_DECL_OVERRIDE;

signals:
    void logAvailable(const QString &);
    void stateChanged(bool);
    void newBytes(quint64);
    void failed();

private:
    bool debug;
    QSS::Profile profile;
};

#endif // CONTROLLERTHREAD_H
