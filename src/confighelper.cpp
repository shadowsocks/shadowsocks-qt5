#include "confighelper.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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

    QStringList headerLabels = QStringList() << tr("Name") << tr("Latency (ms)") << tr("Last used");

    model = new QStandardItemModel(0, 3, this);
    model->setHorizontalHeaderLabels(headerLabels);
    readConfiguration();
}

ConfigHelper::~ConfigHelper()
{
    save();
}

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

    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
}

void ConfigHelper::importGuiConfigJson(const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        return;
    }
    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    if (CONFArray.isEmpty()) {
        qWarning() << "configs in " << file << " is empty.";
        return;
    }

    for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
        QJsonObject json = (*it).toObject();
        SQProfile p;
        /*
         * shadowsocks-csharp uses remarks to store profile name, which is different from
         * old shadowsocks-qt5's implementation. It also uses int to store ports directly
         * and it doesn't have some certain keys.
         */
        if (json.contains("remarks")) {
            p.name = json["remarks"].toString();
            p.serverPort = json["server_port"].toInt();
            //shadowsocks-csharp has only global local port (all profiles use the same port)
            p.localPort = JSONObj["localPort"].toInt();
            if (JSONObj["shareOverLan"].toBool()) {
                /*
                 * it can only configure share over LAN or not (also a global value)
                 * which is basically 0.0.0.0 or 127.0.0.1 (which is the default)
                 */
                p.localAddress = QString("0.0.0.0");
            }
        } else {
            p.name = json["profile"].toString();
            p.serverPort = json["server_port"].toString().toUShort();
            p.localAddress = json["local_address"].toString();
            p.localPort = json["local_port"].toString().toUShort();
            p.timeout = json["timeout"].toString().toInt();
        }
        p.serverAddress = json["server"].toString();
        p.method = json["method"].toString();
        p.password = json["password"].toString();
        Connection *con = new Connection(p, this);
        appendConnectionToList(con);
    }
}

Connection* ConfigHelper::configJsonToConnection(const QString &file)
{
    QFile JSONFile(file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!JSONFile.isOpen()) {
        qCritical() << "Error: cannot open " << file;
    }
    if(!JSONFile.isReadable()) {
        qCritical() << "Error: cannot read " << file;
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);
    JSONFile.close();
    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }
    if (JSONDoc.isEmpty()) {
        qCritical() << "JSON Document" << file << "is empty!";
        return nullptr;
    }
    QJsonObject configObj = JSONDoc.object();
    SQProfile p;
    p.serverAddress = configObj["server"].toString();
    p.serverPort = configObj["server_port"].toInt();
    p.localAddress = configObj["local_address"].toString();
    p.localPort = configObj["local_port"].toInt();
    p.method = configObj["method"].toString();
    p.password = configObj["password"].toString();
    p.timeout = configObj["timeout"].toInt();
    Connection *con = new Connection(p, this);
    return con;
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
    model->setData(model->index(row, 0), QVariant(con->profile.name));
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

void ConfigHelper::latencyTestAtRow(int row)
{
    Connection *con = model->data(model->index(row, 0), Qt::UserRole).value<Connection *>();
    con->latencyTest();
}

QVariant ConfigHelper::convertLatencyToVariant(const int latency)
{
    QVariant latencyData;
    switch (latency) {
    case -1:
        latencyData = QVariant(3000);//>= 3000 ms: timeout
        break;
    case -3://unknown
        latencyData = QVariant(0);
        break;
    case -2://error
    default:
        latencyData = QVariant(latency);
    }
    return latencyData;
}

void ConfigHelper::testAllLatency()
{
    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model->data(model->index(i, 0), Qt::UserRole).value<Connection *>();
        con->latencyTest();
    }
}

int ConfigHelper::getToolbarStyle() const
{
    return toolbarStyle;
}

bool ConfigHelper::isHideWindowOnStartup() const
{
    return hideWindowOnStartup;
}

bool ConfigHelper::isOnlyOneInstance() const
{
    return onlyOneInstace;
}

void ConfigHelper::setGeneralSettings(int ts, bool hide, bool oneInstance)
{
    if (toolbarStyle != ts) {
        emit toolbarStyleChanged(static_cast<Qt::ToolButtonStyle>(ts));
    }
    toolbarStyle = ts;
    hideWindowOnStartup = hide;
    onlyOneInstace = oneInstance;
}

int ConfigHelper::size() const
{
    return model->rowCount();
}

QModelIndex ConfigHelper::moveUp(int row)
{
    QList<QStandardItem*> src = model->takeRow(row);
    model->insertRow(row - 1, src);
    return model->index(row - 1, 0);
}

QModelIndex ConfigHelper::moveDown(int row)
{
    QList<QStandardItem*> src = model->takeRow(row);
    model->insertRow(row + 1, src);
    return model->index(row + 1, 0);
}

void ConfigHelper::appendConnectionToList(Connection *con)
{
    connect(con, &Connection::stateChanged, this, &ConfigHelper::onConnectionStateChanged);
    connect(con, &Connection::pingFinished, this, &ConfigHelper::onConnectionPingFinished);
    connect(con, &Connection::startFailed, this, &ConfigHelper::connectionStartFailed);
    QList<QStandardItem *> items;
    QStandardItem *name = new QStandardItem();
    name->setData(QVariant(con->profile.name), Qt::DisplayRole);
    name->setData(QVariant::fromValue(con), Qt::UserRole);
    QStandardItem *latency = new QStandardItem;
    latency->setData(convertLatencyToVariant(con->profile.latency), Qt::DisplayRole);
    QStandardItem *last = new QStandardItem(con->profile.lastTime.toString());
    items << name << latency << last;
    model->appendRow(items);
}

void ConfigHelper::readConfiguration()
{
    int size = settings->beginReadArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariant value = settings->value("SQProfile");
        SQProfile profile = value.value<SQProfile>();
        checkProfileDataUsageReset(profile);
        Connection *con = new Connection(profile, this);
        appendConnectionToList(con);
    }
    settings->endArray();

    toolbarStyle = settings->value("ToolbarStyle", QVariant(4)).toInt();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
}

void ConfigHelper::checkProfileDataUsageReset(SQProfile &profile)
{
    QDate currentDate = QDate::currentDate();
    if (profile.nextResetDate.isNull()){//invalid if the config.ini is old
        //the default reset day is 1 of every month
        profile.nextResetDate = QDate(currentDate.year(), currentDate.month(), 1);
        qDebug() << "config.ini upgraded from old version";
        profile.totalUsage += profile.currentUsage;//we used to use sent and received
    }

    if (profile.nextResetDate < currentDate) {//not <= because that'd casue multiple reset on this day
        profile.currentUsage = 0;
        while (profile.nextResetDate <= currentDate) {
            profile.nextResetDate = profile.nextResetDate.addMonths(1);
        }
    }
}

void ConfigHelper::onConnectionStateChanged(bool running)
{
    Connection *c = qobject_cast<Connection*>(sender());
    if (!c) {
        return;
    }

    if (running) {
        emit message(c->getName() + " " + tr("connected"));
    } else {
        emit message(c->getName() + " " + tr("disconnected"));
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
    if(row == size) {
        //row doesn't exist (already deleted)
        return;
    }
    int cols = model->columnCount();
    for (int i = 0; i < cols; ++i) {
        model->item(row, i)->setFont(font);
    }
    emit rowStatusChanged(row, running);
}

void ConfigHelper::onConnectionPingFinished(const int latency)
{
    Connection *c = qobject_cast<Connection *>(sender());
    if (!c) {
        return;
    }

    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model->data(model->index(i, 0), Qt::UserRole).value<Connection *>();
        if (con == c) {
            model->setData(model->index(i, 1), convertLatencyToVariant(latency));
            break;
        }
    }

    if (latency == -1) {//TIMEOUT
        emit message(c->getName() + " " + tr("timed out"));
    } else if (latency == -2) {//ERROR
        emit message(c->getName() + " " + tr("latency test failed"));
    }
}

void ConfigHelper::startAllAutoStart()
{
    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model->data(model->index(i, 0), Qt::UserRole).value<Connection *>();
        if (con->profile.autoStart) {
            con->start();
            //update time
            model->setData(model->index(i, 2), QVariant(con->profile.lastTime.toString()));
        }
    }
}
