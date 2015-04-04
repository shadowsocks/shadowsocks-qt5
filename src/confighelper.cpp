#include "confighelper.h"
#include <QDir>
#include <QCoreApplication>

ConfigHelper::ConfigHelper(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN
    configFile = QCoreApplication::applicationDirPath() + "/config.ini";
#else
    QDir configDir = QDir::homePath() + "/.config/shadowsocks-qt5";
    configFile = configDir.absolutePath() + "/config.ini";
    if (!configDir.exists()) {
        configDir.mkpath(configDir.absolutePath());
    }
#endif

    settings = new QSettings(configFile, QSettings::IniFormat, this);

    model = new QStandardItemModel(0, 3, this);
    model->setHorizontalHeaderLabels(headerLabels);
    readConfiguration();
    fillModel();
}

ConfigHelper::~ConfigHelper()
{
    save();
}

const QStringList ConfigHelper::headerLabels = QStringList() << tr("Name") << tr("Lag") << tr("Last used");

const QString ConfigHelper::profilePrefix = "Profile";

QStandardItemModel *ConfigHelper::getModel() const
{
    return model;
}

void ConfigHelper::save()
{
    int size = connectionList.size();
    settings->beginWriteArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = QVariant::fromValue<SQProfile>(connectionList.at(i)->getProfile());
        settings->setValue("SQProfile", value);
    }
    settings->endArray();
}

void ConfigHelper::addConnection(Connection *con)
{
    con->setParent(this);
    connectionList.append(con);
    appendConnectionToList(con);
}

void ConfigHelper::deleteRow(int row)
{
    Connection *removed = model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
    connectionList.removeAll(removed);
    model->removeRow(row);
}

void ConfigHelper::appendConnectionToList(Connection *con)
{
    QList<QStandardItem *> items;
    QStandardItem *name = new QStandardItem();
    name->setData(QVariant(con->profile.name), Qt::DisplayRole);
    name->setData(QVariant::fromValue(con), Qt::UserRole);
    QStandardItem *lag = new QStandardItem(QString::number(con->profile.lag));
    QStandardItem *last = new QStandardItem(con->profile.lastTime.toString());
    items << name << lag << last;
    model->appendRow(items);
}

void ConfigHelper::readConfiguration()
{
    int size = settings->beginReadArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("SQProfile");
        SQProfile profile = value.value<SQProfile>();
        Connection *con = new Connection(profile, this);
        connectionList.append(con);
    }
    settings->endArray();
}

void ConfigHelper::fillModel()
{
    for (auto it = connectionList.begin(); it != connectionList.end(); ++it) {
        appendConnectionToList(*it);
    }
}
