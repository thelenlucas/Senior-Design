#ifndef LOGTABLEMODEL_H
#define LOGTABLEMODEL_H

#include <QAbstractTableModel>
#include <vector>
#include "logs.hpp"

class LogTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit LogTableModel(QObject *parent = nullptr);

    // Set or update the logs data.
    void setLogs(const std::vector<Log>& logs);

    // QAbstractItemModel overrides:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<Log> m_logs;
};

#endif // LOGTABLEMODEL_H