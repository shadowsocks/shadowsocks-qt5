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
        QList<QStandardItem *> items;
        QStandardItem *name = new QStandardItem((*it)->profile.name);
        QStandardItem *lag = new QStandardItem(QString::number((*it)->profile.lag));
        QStandardItem *last = new QStandardItem((*it)->profile.lastTime.toString());
        items << name << lag << last;
        model->appendRow(items);
    }
}
