#include "confighelper.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

ConfigHelper::ConfigHelper(ConnectionTableModel *model, QObject *parent) :
    QObject(parent),
    model(model)
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
    readConfiguration();
}

const QString ConfigHelper::profilePrefix = "Profile";

void ConfigHelper::save()
{
    int size = model->rowCount();
    settings->beginWriteArray(profilePrefix);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        Connection *con = model->getItem(i)->getConnection();
        QVariant value = QVariant::fromValue<SQProfile>(con->getProfile());
        settings->setValue("SQProfile", value);
    }
    settings->endArray();

    settings->setValue("ToolbarStyle", QVariant(toolbarStyle));
    settings->setValue("HideWindowOnStartup", QVariant(hideWindowOnStartup));
    settings->setValue("OnlyOneInstance", QVariant(onlyOneInstace));
    settings->setValue("ShowToolbar", QVariant(showToolbar));
    settings->setValue("ShowFilterBar", QVariant(showFilterBar));
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
        model->appendConnection(con);
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

void ConfigHelper::latencyTestAtRow(int row)
{
    Connection *con = model->getItem(row)->getConnection();
    con->latencyTest();
}

void ConfigHelper::testAllLatency()
{
    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model->getItem(i)->getConnection();
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

bool ConfigHelper::isShowToolbar() const
{
    return showToolbar;
}

bool ConfigHelper::isShowFilterBar() const
{
    return showFilterBar;
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

void ConfigHelper::setShowToolbar(bool show)
{
    showToolbar = show;
}

void ConfigHelper::setShowFilterBar(bool show)
{
    showFilterBar = show;
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
        model->appendConnection(con);
    }
    settings->endArray();

    toolbarStyle = settings->value("ToolbarStyle", QVariant(4)).toInt();
    hideWindowOnStartup = settings->value("HideWindowOnStartup").toBool();
    onlyOneInstace = settings->value("OnlyOneInstance", QVariant(true)).toBool();
    showToolbar = settings->value("ShowToolbar", QVariant(true)).toBool();
    showFilterBar = settings->value("ShowFilterBar", QVariant(true)).toBool();
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

void ConfigHelper::startAllAutoStart()
{
    int size = model->rowCount();
    for (int i = 0; i < size; ++i) {
        Connection *con = model->getItem(i)->getConnection();
        if (con->profile.autoStart) {
            con->start();
            //update time
            model->setData(model->index(i, 2), QVariant(con->profile.lastTime.toString()));
        }
    }
}
