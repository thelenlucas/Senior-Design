#include "logtablemodel.hpp"
#include <QString>

LogTableModel::LogTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void LogTableModel::setLogs(const std::vector<Log> &logs)
{
    beginResetModel();
    m_logs = logs;
    endResetModel();
}

int LogTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_logs.size());
}

int LogTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // id, species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes
    return 8;
}

QVariant LogTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();
    int col = index.column();

    if (row >= static_cast<int>(m_logs.size()))
        return QVariant();

    const Log &log = m_logs[row];

    switch (col) {
    case 0:
        return log.getId();
    case 1:
        return QString::fromStdString(log.getSpecies());
    case 2:
        return (log.getLenQuarters() / 4.0 / 12.0);
    case 3:
        return (log.getDiameterQuarters() / 4.0);
    case 4:
        return (log.getCostCentsQuarters() / 100.0 * log.getLenQuarters());
    case 5:
        return static_cast<int>(log.getQuality());
    case 6:
        return QString::fromStdString(log.getLocation());
    case 7:
        return QString::fromStdString(log.getNotes());
    default:
        return QVariant();
    }
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return QString("ID");
        case 1: return QString("Species");
        case 2: return QString("Length (ft)");
        case 3: return QString("Diameter (in)");
        case 4: return QString("Cost ($)");
        case 5: return QString("Quality");
        case 6: return QString("Location");
        case 7: return QString("Notes");
        default: return QVariant();
        }
    }
    return QVariant();
}