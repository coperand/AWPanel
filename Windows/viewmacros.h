#ifndef VIEWMACROS_H
#define VIEWMACROS_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QTableView>
#include <QVector>

#include "netthread.h"
#include "tablemodel.h"
#include "delbutton.h"

class ViewMacros : public QWidget
{
    Q_OBJECT
public:
    ViewMacros(QWidget *parent, NetThread *net);

private:
    TableModel *model;
    QTableView *table;
    NetThread *net;

public slots:
    void updateVector();
};

#endif // VIEWMACROS_H
