/*
 * Copyright (C) 2015-2016 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QObject>
#include <QStringList>
#include "connection.h"

class ConnectionItem : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionItem(Connection *_con, QObject *parent = nullptr);

    static int columnCount();
    QVariant data(int column, int role = Qt::DisplayRole) const;

    Connection* getConnection();
    void testLatency();

signals:
    void message(const QString&);
    void stateChanged(bool);
    void dataUsageChanged(const quint64 &current, const quint64 &total);
    void latencyChanged();

private:
    Connection *con;

    static QString convertLatencyToString(const int latency);
    static QString convertBytesToHumanReadable(quint64 bytes);
    static const QStringList bytesUnits;

private slots:
    void onConnectionStateChanged(bool running);
    void onConnectionPingFinished(const int latency);
    void onStartFailed();
};

#endif // CONNECTIONITEM_H
