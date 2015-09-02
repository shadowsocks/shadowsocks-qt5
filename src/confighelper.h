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
#include <QList>
#include "connectiontablemodel.h"
#include "connection.h"

class ConfigHelper : public QObject
{
    Q_OBJECT

public:
    ConfigHelper(ConnectionTableModel *model, QObject *parent);

    void importGuiConfigJson(const QString &file);
    Connection* configJsonToConnection(const QString &file);
    //The parent of con will be this ConfigHelper instance
    void latencyTestAtRow(int row);//perform a latency test and update it in model
    void startAllAutoStart();//start those connections marked as auto-start

    /* some functions used to communicate with SettingsDialog */
    int  getToolbarStyle() const;
    bool isHideWindowOnStartup() const;
    bool isOnlyOneInstance() const;
    bool isShowToolbar() const;
    bool isShowFilterBar() const;
    void setGeneralSettings(int ts, bool hide, bool oneInstance);

public slots:
    void testAllLatency();
    void save();
    void setShowToolbar(bool show);
    void setShowFilterBar(bool show);

signals:
    void toolbarStyleChanged(const Qt::ToolButtonStyle);

private:
    int toolbarStyle;
    bool hideWindowOnStartup;
    bool onlyOneInstace;
    bool showToolbar;
    bool showFilterBar;
    QSettings *settings;
    ConnectionTableModel *model;
    QString configFile;

    void readConfiguration();
    void checkProfileDataUsageReset(SQProfile &profile);

    static const QString profilePrefix;
};

#endif // CONFIGHELPER_H
