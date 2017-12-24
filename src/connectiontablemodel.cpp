#include "connectiontablemodel.h"

ConnectionTableModel::ConnectionTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{}

ConnectionTableModel::~ConnectionTableModel()
{}

ConnectionItem *ConnectionTableModel::getItem(const int &row) const
{
    return items.at(row);
}

int ConnectionTableModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int ConnectionTableModel::columnCount(const QModelIndex &) const
{
    return ConnectionItem::columnCount();
}

QVariant ConnectionTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    ConnectionItem *item = getItem(index.row());
    return item->data(index.column(), role);
}

QVariant ConnectionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return QVariant(tr("Name"));
    case 1:
        return QVariant(tr("Server"));
    case 2:
        return QVariant(tr("Status"));
    case 3:
        return QVariant(tr("Latency"));
    case 4:
        return QVariant(tr("Local Port"));
    case 5:
        return QVariant(tr("Term Usage"));
    case 6:
        return QVariant(tr("Total Usage"));
    case 7:
        return QVariant(tr("Reset Date"));
    case 8:
        return QVariant(tr("Last Used"));
    default:
        return QVariant();
    }
}

QModelIndex ConnectionTableModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= items.size()) {
        return QModelIndex();
    } else {
        ConnectionItem* item = items.at(row);
        return createIndex(row, column, item);//column is ignored (all columns have the same effect)
    }
}

bool ConnectionTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count <= 0 || count + row > items.count()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    items.erase(items.begin() + row, items.begin() + row + count);
    endRemoveRows();
    return true;
}

bool ConnectionTableModel::move(int row, int target, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount() || target < 0 || target >= rowCount() || row == target) {
        return false;
    }

    //http://doc.qt.io/qt-5/qabstractitemmodel.html#beginMoveRows
    int movTarget = target;
    if (target - row > 0) {
        movTarget++;
    }
    beginMoveRows(parent, row, row, parent, movTarget);
    items.move(row, target);
    endMoveRows();
    return true;
}

bool ConnectionTableModel::appendConnection(Connection *con, const QModelIndex &parent)
{
    ConnectionItem* newItem = new ConnectionItem(con, this);
    connect(newItem, &ConnectionItem::message, this, &ConnectionTableModel::message);
    connect(newItem, &ConnectionItem::stateChanged, this, &ConnectionTableModel::onConnectionStateChanged);
    connect(newItem, &ConnectionItem::latencyChanged, this, &ConnectionTableModel::onConnectionLatencyChanged);
    connect(newItem, &ConnectionItem::dataUsageChanged, this, &ConnectionTableModel::onConnectionDataUsageChanged);
    beginInsertRows(parent, items.count(), items.count());
    items.append(newItem);
    endInsertRows();
    return true;
}

void ConnectionTableModel::disconnectConnectionsAt(const QString &addr, quint16 port)
{
    bool anyAddr = (addr.compare("0.0.0.0") == 0);
    for (auto &i : items) {
        Connection *con = i->getConnection();
        if (con->isRunning() && con->getProfile().localPort == port) {
            if ((con->getProfile().localAddress == addr) ||
                (con->getProfile().localAddress.compare("0.0.0.0") == 0) ||
                anyAddr) {
                con->stop();
            }
        }
    }
}

void ConnectionTableModel::testAllLatency()
{
    for (auto &i : items) {
        i->testLatency();
    }
}

void ConnectionTableModel::onConnectionStateChanged(bool running)
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 0),
                     this->index(row, ConnectionItem::columnCount() - 1));
    emit rowStatusChanged(row, running);
}

void ConnectionTableModel::onConnectionLatencyChanged()
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 3), this->index(row, 3));
}

void ConnectionTableModel::onConnectionDataUsageChanged()
{
    ConnectionItem *item = qobject_cast<ConnectionItem*>(sender());
    int row = items.indexOf(item);
    emit dataChanged(this->index(row, 5), this->index(row, 6));
}