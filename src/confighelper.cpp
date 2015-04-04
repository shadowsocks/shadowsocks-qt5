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

    model = new QStandardItemModel(0, 4, this);
    model->setHorizontalHeaderLabels(headerLabels);
    readConfiguration();
}

ConfigHelper::~ConfigHelper()
{
    save();
}

const QStringList ConfigHelper::headerLabels = QStringList() << tr("Name") << tr("Status") << tr("Lag") << tr("Last used");

QStandardItemModel *ConfigHelper::getModel() const
{
    return model;
}

void ConfigHelper::save()
{
    //TODO save all profiles back
}

void ConfigHelper::readConfiguration()
{
    //TODO parse JSON file, fill in connectionList
}

void ConfigHelper::fillModel()
{
    //TODO use connectionList to fill model
}
