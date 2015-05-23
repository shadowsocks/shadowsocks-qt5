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

#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QSettings>
#include <QStandardItemModel>
#include <QList>
#include "connection.h"

class ConfigHelper : public QObject
{
    Q_OBJECT

public:
    ConfigHelper(QObject *parent);
    ~ConfigHelper();

    QStandardItemModel *getModel() const;

    void importGuiConfigJson(const QString &file);
    Connection* configJsonToConnection(const QString &file);
    //The parent of con will be this ConfigHelper instance
    void addConnection(Connection *con);
    void deleteRow(int row);
    void updateNameAtRow(int row);
    void updateTimeAtRow(int row);
    Connection *connectionAt(int row);
    void latencyTestAtRow(int row);//perform a latency test and update it in model
    void startAllAutoStart();//start those connections marked as auto-start

    /* some functions used to communicate with SettingsDialog */
    int  getToolbarStyle() const;
    bool isHideWindowOnStartup() const;
    bool isOnlyOneInstance() const;
    void setGeneralSettings(int ts, bool hide, bool oneInstance);

    int size() const;
    QModelIndex moveUp(int row);
    QModelIndex moveDown(int row);

public slots:
    void testAllLatency();
    void save();

signals:
    void toolbarStyleChanged(const Qt::ToolButtonStyle);
    void rowStatusChanged(const int row, const bool running);
    void connectionStartFailed();
    void message(const QString &msg);

private:
    int toolbarStyle;
    bool hideWindowOnStartup;
    bool onlyOneInstace;
    QSettings *settings;
    QStandardItemModel *model;
    QString configFile;

    void appendConnectionToList(Connection *con);
    void readConfiguration();
    void checkProfileDataUsageReset(SQProfile &profile);

    static const QString profilePrefix;

    static QVariant convertLatencyToVariant(const int latency);

private slots:
    void onConnectionStateChanged(bool running);
    void onConnectionPingFinished(const int latency);
};

#endif // CONFIGHELPER_H
