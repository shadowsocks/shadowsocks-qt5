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
        return QVariant(tr("Status"));
    case 2:
        return QVariant(tr("Latency (ms)"));
    case 3:
        return QVariant(tr("Local Port"));
    case 4:
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

bool ConnectionTableModel::removeRows(int row, int count, const QModelIndex &)
{
    if (row < 0 || count <= 0 || count + row > items.count()) {
        return false;
    }
    items.erase(items.begin() + row, items.begin() + row + count);
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
    beginInsertRows(parent, items.count(), items.count() + 1);
    items.append(newItem);
    endInsertRows();
    return true;
}

void ConnectionTableModel::testAllLatency()
{
    for (auto &i : items) {
        i->testLatency();
    }
}

void ConnectionTableModel::onConnectionStateChanged(bool running)
{
    ConnectionItem* item = qobject_cast<ConnectionItem*>(sender());
    emit rowStatusChanged(items.indexOf(item), running);
}
