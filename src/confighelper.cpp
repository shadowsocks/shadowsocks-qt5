#include "confighelper.h"
#include <QDir>

ConfigHelper::ConfigHelper(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN
    jsonconfigFile = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir ssConfigDir = QDir::homePath() + "/.config/shadowsocks-qt5";
    jsonconfigFile = ssConfigDir.absolutePath() + "/config.json";
    if (!ssConfigDir.exists()) {
        ssConfigDir.mkpath(ssConfigDir.absolutePath());
    }
#endif

    model = new QStandardItemModel(0, 4, this);
    model->setHorizontalHeaderLabels(headerLabels);
    readConfiguration();
}

ConfigHelper::~ConfigHelper()
{
}

const QStringList ConfigHelper::headerLabels = QStringList() << tr("Name") << tr("Status") << tr("Lag") << tr("Last used");

QStandardItemModel *ConfigHelper::getModel() const
{
    return model;
}

void ConfigHelper::save()
{
    //TODO save all profiles back to json
}

void ConfigHelper::readConfiguration()
{
    //TODO parse JSON file, fill in connectionList
}

void ConfigHelper::fillModel()
{
    //TODO use connectionList to fill model
}
