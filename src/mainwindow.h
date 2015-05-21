/*
 * Copyright (C) 2014-2015 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "confighelper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool isOnlyOneInstance() const;
    bool isHideWindowOnStartup() const;

signals:
    void messageArrived(const QString& msg);

private:
    ConfigHelper *configHelper;
    Ui::MainWindow *ui;

    void newProfile(Connection *);
    void editRow(int row);
    void blockChildrenSignals(bool);

    static const QUrl issueUrl;

private slots:
    void onImportGuiJson();
    void onAddManually();
    void onAddScreenQRCode();
    void onAddQRCodeFile();
    void onAddFromURI();
    void onAddFromConfigJSON();
    void onDelete();
    void onEdit();
    void onDoubleClicked(const QModelIndex &index);
    void onShare();
    void onConnect();
    void onDisconnect();
    void onConnectionStatusChanged(const int row, const bool running);
    void onLatencyTest();
    void onViewLog();
    void onStatus();
    void onMoveUp();
    void onMoveDown();
    void onGeneralSettings();
    void checkCurrentIndex(const QModelIndex &index);
    void onAbout();
    void onReportBug();
    void onCustomContextMenuRequested(const QPoint &pos);

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
