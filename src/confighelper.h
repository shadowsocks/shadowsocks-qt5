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
    void exportGuiConfigJson(const QString &file);//the format is only compatible with shadowsocks-csharp
    Connection* configJsonToConnection(const QString &file);
    void startAllAutoStart();//start those connections marked as auto-start

    /* some functions used to communicate with SettingsDialog */
    int  getToolbarStyle() const;
    bool isHideWindowOnStartup() const;
    bool isOnlyOneInstance() const;
    bool isShowToolbar() const;
    bool isShowFilterBar() const;
    bool isNativeMenuBar() const;
    void setGeneralSettings(int ts, bool hide, bool oneInstance, bool nativeMB);

public slots:
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
    bool nativeMenuBar;
    QSettings *settings;
    ConnectionTableModel *model;
    QString configFile;

    void readConfiguration();
    void checkProfileDataUsageReset(SQProfile &profile);

    static const QString profilePrefix;
};

#endif // CONFIGHELPER_H
