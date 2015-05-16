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
    bool isHideWindowOnStartup() const;
    bool isOnlyOneInstance() const;
    void setGeneralSettings(bool hide, bool oneInstance);

    int size() const;
    QModelIndex moveUp(int row);
    QModelIndex moveDown(int row);

public slots:
    void testAllLatency();
    void save();

signals:
    void rowStatusChanged(const int row, const bool running);
    void connectionStartFailed();
    void message(const QString &msg);

private:
    bool hideWindowOnStartup;
    bool onlyOneInstace;
    QSettings *settings;
    QStandardItemModel *model;
    QString configFile;

    void appendConnectionToList(Connection *con);
    void readConfiguration();

    static const QString profilePrefix;

    static QVariant convertLatencyToVariant(const int latency);

private slots:
    void onConnectionStateChanged(bool running);
    void onConnectionPingFinished(const int latency);
};

#endif // CONFIGHELPER_H
