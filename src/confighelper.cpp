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
    int size = model->rowCount();
    settings->beginWriteArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Connection *con = model->data(model->index(i, 0), Qt::UserRole).value<Connection *>();
        QVariant value = QVariant::fromValue<SQProfile>(con->getProfile());
        settings->setValue("SQProfile", value);
    }
    settings->endArray();

    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
}

void ConfigHelper::addConnection(Connection *con)
{
    con->setParent(this);
    appendConnectionToList(con);
}

void ConfigHelper::deleteRow(int row)
{
    Connection *removed = model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
    removed->deleteLater();
    model->removeRow(row);
}

void ConfigHelper::updateNameAtRow(int row)
{
    Connection *con = model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
    model->setData(model->index(row, 0), QVariant(con->profile.name), Qt::DisplayRole);
}

void ConfigHelper::updateTimeAtRow(int row)
{
    Connection *con = model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
    model->setData(model->index(row, 2), QVariant(con->profile.lastTime.toString()));
}

Connection* ConfigHelper::connectionAt(int row)
{
    return model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return hideWindowOnStartup;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return onlyOneInstace;
}

void ConfigHelper::setGeneralSettings(bool hide, bool oneInstance)
{
    hideWindowOnStartup = hide;
    onlyOneInstace = oneInstance;
}

void ConfigHelper::appendConnectionToList(Connection *con)
{
    connect(con, &Connection::stateChanged, this, &ConfigHelper::onConnectionStateChanged);
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
        appendConnectionToList(con);
    }
    settings->endArray();

    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance").toBool();
}

void ConfigHelper::onConnectionStateChanged(bool running)
{
    Connection *c = qobject_cast<Connection*>(sender());
    if (!c) {
        return;
    }

    QFont font;
    font.setBold(running);

    int size = model->rowCount();
    int row = 0;
    for (; row < size; ++row) {
        if(model->data(model->index(row, 0), Qt::UserRole).value<Connection *>() == c) {
            break;
        }
    }
    int cols = model->columnCount();
    for (int i = 0; i < cols; ++i) {
        model->item(row, i)->setFont(font);
    }
}
