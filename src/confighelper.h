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
    void save();

    //The parent of con will be this ConfigHelper instance
    void addConnection(Connection *con);
    void deleteRow(int row);
    void updateNameAtRow(int row);
    void updateTimeAtRow(int row);
    Connection *connectionAt(int row);

    /* some functions used to communicate with SettingsDialog */
    bool isHideWindowOnStartup() const;
    bool isOnlyOneInstance() const;
    void setGeneralSettings(bool hide, bool oneInstance);

signals:
    void connected(const QString &name);
    void disconnected(const QString &name);

private:
    bool hideWindowOnStartup;
    bool onlyOneInstace;
    QSettings *settings;
    QStandardItemModel *model;
    QString configFile;

    void appendConnectionToList(Connection *con);
    void readConfiguration();

    static const QStringList headerLabels;
    static const QString profilePrefix;

private slots:
    void onConnectionStateChanged(bool running);
};

#endif // CONFIGHELPER_H
