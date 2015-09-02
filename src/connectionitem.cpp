#include "connectionitem.h"

ConnectionItem::ConnectionItem(Connection *_con, QObject *parent) :
    QObject(parent),
    con(_con)
{
    if (con) {
        con->setParent(this);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::onConnectionStateChanged);
        connect(con, &Connection::stateChanged, this, &ConnectionItem::stateChanged);
        connect(con, &Connection::pingFinished, this, &ConnectionItem::onConnectionPingFinished);
        connect(con, &Connection::startFailed, this, &ConnectionItem::onStartFailed);
    }
}

int ConnectionItem::columnCount()
{
    return 5;
}

QVariant ConnectionItem::data(int column, int role) const
{
    if (!con) {
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return QVariant();
    }

    switch (column) {
    case 0://name
        return QVariant(con->profile.name);
    case 1://status
        if (role == Qt::DisplayRole) {
            return con->isRunning() ? QVariant(tr("Connected")) : QVariant(tr("Disconnected"));
        } else {
            return QVariant(con->isRunning());
        }
    case 2://latency
        if (role == Qt::DisplayRole) {
            return QVariant(convertLatencyToString(con->profile.latency));
        } else {
            return QVariant(con->profile.latency);
        }
    case 3://local port
        return QVariant(con->profile.localPort);
    case 4://last used
        return QVariant(con->profile.lastTime);
    default:
        return QVariant();
    }
}

QString ConnectionItem::convertLatencyToString(const int latency)
{
    QString latencyStr;
    switch (latency) {
    case -1:
        latencyStr = tr("Timeout");
        break;
    case -3://unknown
        latencyStr = tr("Unknown");
        break;
    case -2://error
        latencyStr = tr("Error");
        break;
    default:
        latencyStr = QString::number(latency);
    }
    return latencyStr;
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
    if (latency == -1) {//TIMEOUT
        emit message(con->getName() + " " + tr("timed out"));
    } else if (latency == -2) {//ERROR
        emit message(con->getName() + " " + tr("latency test failed"));
    }
}

void ConnectionItem::onStartFailed()
{
    emit message(tr("Failed to start") + " " + con->getName());
}
