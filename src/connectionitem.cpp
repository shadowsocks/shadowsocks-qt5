#include "connectionitem.h"
#include <QFont>
#include <cmath>

ConnectionItem::ConnectionItem(Connection *_con, QObject *parent) :
    QObject(parent),
    con(_con)
{
    if (con) {
        con->setParent(this);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::onConnectionStateChanged);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::stateChanged);
        connect(con, &Connection::dataUsageChanged, this, &ConnectionItem::dataUsageChanged);
        connect(con, &Connection::latencyAvailable, this, &ConnectionItem::onConnectionPingFinished);
        connect(con, &Connection::latencyAvailable, this, &ConnectionItem::latencyChanged);
        connect(con, &Connection::startFailed, this, &ConnectionItem::onStartFailed);
    }
}

const QStringList ConnectionItem::bytesUnits = QStringList()
        << " B" << " KiB" << " MiB" << " GiB" << " TiB"
        << " PiB" << " EiB" << " ZiB" << " YiB";

int ConnectionItem::columnCount()
{
    return 9;
}

QVariant ConnectionItem::data(int column, int role) const
{
    if (!con) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case 0://name
            return QVariant(con->profile.name);
        case 1://server
            return QVariant(con->profile.serverAddress);
        case 2://status
            return con->isRunning() ? QVariant(tr("Connected"))
                                    : QVariant(tr("Disconnected"));
        case 3://latency
            if (role == Qt::DisplayRole) {
                return QVariant(convertLatencyToString(con->profile.latency));
            } else {
                return QVariant(con->profile.latency);
            }
        case 4://local port
            return QVariant(con->profile.localPort);
        case 5://data usage (term)
            if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.currentUsage));
            } else {
                return QVariant(con->profile.currentUsage);
            }
        case 6://data usage (total)
            if (role == Qt::DisplayRole) {
                return QVariant(convertBytesToHumanReadable(con->profile.totalUsage));
            } else {
                return QVariant(con->profile.totalUsage);
            }
        case 7://reset date
            if (role == Qt::DisplayRole) {
                return QVariant(con->profile.nextResetDate.toString(Qt::SystemLocaleShortDate));
            } else {
                return QVariant(con->profile.nextResetDate);
            }
        case 8://last used
            if (role == Qt::DisplayRole) {
                return QVariant(con->profile.lastTime.toString(Qt::SystemLocaleShortDate));
            } else {
                return QVariant(con->profile.lastTime);
            }
        default:
            return QVariant();
        }
    } else if (role == Qt::FontRole) {
        QFont font;
        font.setBold(con->isRunning());
        return QVariant(font);
    }

    return QVariant();
}

QString ConnectionItem::convertLatencyToString(const int latency)
{
    QString latencyStr;
    switch (latency) {
    case SQProfile::LATENCY_TIMEOUT:
        latencyStr = tr("Timeout");
        break;
    case SQProfile::LATENCY_ERROR:
        latencyStr = tr("Error");
        break;
    case SQProfile::LATENCY_UNKNOWN:
        latencyStr = tr("Unknown");
        break;
    default:
        if (latency >= 1000) {
            latencyStr = QString::number(static_cast<double>(latency) / 1000.0)
                       + QStringLiteral(" ") + tr("s");
        } else {
            latencyStr = QString::number(latency) + QStringLiteral(" ") + tr("ms");
        }
    }
    return latencyStr;
}

QString ConnectionItem::convertBytesToHumanReadable(quint64 quot)
{
    int unitId = 0;
    quint64 rem = 0;
    for (; quot > 1024; ++unitId) {
        rem = quot % 1024;//the previous rem would be negligible
        quot /= 1024;
    }
    double output = static_cast<double>(quot)
                  + static_cast<double>(rem) / 1024.0;
    return QString("%1 %2").arg(output, 0, 'f', 2).arg(bytesUnits.at(unitId));
}

void ConnectionItem::testLatency()
{
    con->latencyTest();
}

Connection* ConnectionItem::getConnection()
{
    return con;
}

void ConnectionItem::onConnectionStateChanged(bool running)
{
    if (running) {
        emit message(con->getName() + " " + tr("connected"));
    } else {
        emit message(con->getName() + " " + tr("disconnected"));
    }
}

void ConnectionItem::onConnectionPingFinished(const int latency)
{
    if (latency == SQProfile::LATENCY_TIMEOUT) {
        emit message(con->getName() + " " + tr("timed out"));
    } else if (latency == SQProfile::LATENCY_ERROR) {
        emit message(con->getName() + " " + tr("latency test failed"));
    }
}

void ConnectionItem::onStartFailed()
{
    emit message(tr("Failed to start") + " " + con->getName());
}
