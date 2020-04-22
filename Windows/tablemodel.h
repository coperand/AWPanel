#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QTableView>
#include <QtWidgets>

#include "netthread.h"

class TableModel : public QAbstractTableModel
{
public:
    TableModel(QObject *parent = nullptr);
    void setVector(QVector<macro> macros);
    int myRowCount();
    QVector<macro> macros;

private:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // TABLEMODEL_H
