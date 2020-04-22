#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

int TableModel::myRowCount()
{
    return macros.count();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return macros.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

void TableModel::setVector(QVector<macro> macros)
{
    this->macros = macros;
    beginResetModel();
    endResetModel();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole)
    {
        QString str;
        if(index.column() == 0)
        {
            str.setNum(macros[index.row()].code, 16);
            if(str.count() == 1)
                str.insert(0, "0x0");
            else str.insert(0, "0x");
            return QVariant(str);

        } else if (index.column() == 1)
        {
            str = macros[index.row()].description;
            return QVariant(str);
        } else
        {
            return QVariant();
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return QVariant(QString::fromUtf8("Code"));
            case 1:
                return QVariant(QString::fromUtf8("Description"));
            default:
                return QVariant();
            }
        }
        return QVariant();
}
