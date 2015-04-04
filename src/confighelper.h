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

private:
    QSettings *settings;
    QStandardItemModel *model;
    QString configFile;
    QList<Connection*> connectionList;

    void readConfiguration();
    void fillModel();

    static const QStringList headerLabels;
    static const QString profilePrefix;
};

#endif // CONFIGHELPER_H
