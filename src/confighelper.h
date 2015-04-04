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

private:
    QSettings *settings;
    QStandardItemModel *model;
    QString configFile;
    QList<Connection*> connectionList;
    void appendConnectionToList(Connection *con);

    void readConfiguration();
    void fillModel();

    static const QStringList headerLabels;
    static const QString profilePrefix;
};

#endif // CONFIGHELPER_H
