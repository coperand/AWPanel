#ifndef NETTHREAD_H
#define NETTHREAD_H

#include <QUdpSocket>
#include <QObject>
#include <QVector>
#include <QFile>
#include <QDataStream>
#include <QtWidgets/QApplication>

#include <Windows.h>

struct keyEvent
{
    unsigned short code;
    bool up;
};

struct macro
{
    int code;
    QString description;
    QVector<struct keyEvent> vector;
};

class NetThread : public QObject
{
    Q_OBJECT

public:
    NetThread();
    int next_code;
    QVector<struct macro> getVector();

private:
    QUdpSocket *socket;
    void press(QVector<struct keyEvent> vector);
    QVector<macro> macros;
    void setNextCode();

public slots:
    void addMacro(struct macro);
    void delMacro(int);

private slots:
    void readData();

signals:
    void updateTable();
};

#endif // NETTHREAD_H
