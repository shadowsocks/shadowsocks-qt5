/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014-2015 Symeon Huang <hzwhuang@gmail.com>
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
