#include "viewmacros.h"

ViewMacros::ViewMacros(QWidget *parent, NetThread *net) : QWidget(parent)
{
    QVBoxLayout *vertical = new QVBoxLayout();
    table = new QTableView();
    model = new TableModel();

    model->setVector(net->getVector());
    table->setModel(model);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setColumnWidth(0, 15);
    table->setColumnWidth(1, 250);
    table->setColumnWidth(2, 15);

    for(int i = 0; i < model->myRowCount(); i++)
    {
        DelButton *del = new DelButton("del", i);
        table->setIndexWidget(model->index(i, 2), del);
        connect(del, SIGNAL(clickedIndex(int)), net, SLOT(delMacro(int)));
    }

    QPushButton *addBtn = new QPushButton("Add Macro");
    connect(addBtn, SIGNAL(clicked()), parent, SLOT(addMacro()));

    this->net = net;
    connect(net, SIGNAL(updateTable()), this, SLOT(updateVector()));

    vertical->addWidget(table);
    vertical->addWidget(addBtn);

    setLayout(vertical);
}

void ViewMacros::updateVector()
{
    int position = table->rowViewportPosition(table->currentIndex().row());
    model->setVector(net->getVector());
    for(int i = 0; i < model->myRowCount(); i++)
    {
        DelButton *del = new DelButton("del", i);
        table->setIndexWidget(model->index(i, 2), del);
        connect(del, SIGNAL(clickedIndex(int)), net, SLOT(delMacro(int)));
    }

    QScrollBar *scrBar = table->verticalScrollBar();
    scrBar->setValue(scrBar->value() + position);
}
