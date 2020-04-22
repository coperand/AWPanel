#include "netthread.h"

#include <QDebug>

#define PORT 15623

void NetThread::press(QVector<struct keyEvent> vector)
{
    int quantity = vector.count();
    INPUT *action = static_cast<INPUT *>(malloc(sizeof(INPUT) * static_cast<unsigned int>(quantity)));
    for(int i = 0; i < quantity; i++)
    {
        action[i].ki = {};

        action[i].type = INPUT_KEYBOARD;
        action[i].ki.wVk = vector[i].code;
        if(vector[i].up)
            action[i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    SendInput(static_cast<unsigned int>(quantity), action, sizeof(INPUT));
    free(action);
}

NetThread::NetThread()
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::AnyIPv4, PORT);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));

    next_code = 0x01;
    QFile file(QCoreApplication::applicationDirPath() + "/macros.qt");
    if(file.exists() && file.size() != 0)
    {
        QDataStream stream(&file);
        file.open(QIODevice::ReadOnly);
        int count = 0;
        stream >> count;

        for(int i = 0; i < count; i++)
        {
            struct macro mac = {};
            int jcount = 0;
            stream >> mac.code >> mac.description >> jcount;
            for(int j = 0; j < jcount; j++)
            {
                struct keyEvent event = {};
                stream >> event.code >> event.up;
                mac.vector.push_back(event);
            }
            macros.push_back(mac);
        }

        stream >> next_code;
        if(next_code == 0)
            setNextCode();
        file.close();
    }
}

void NetThread::setNextCode()
{
    bool found;
    for(int i = 0x01; i < 0xFF; i++)
    {
        found = false;
        foreach(struct macro mac, macros)
        {
            if(mac.code == i)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            next_code = i;
            break;
        }
    }
}

void NetThread::readData()
{
    while(socket->hasPendingDatagrams())
    {
        QByteArray datagram, send_data;
        datagram.resize(static_cast<int>(socket->pendingDatagramSize()));
        QHostAddress *address = new QHostAddress();
        quint16 port;
        socket->readDatagram(datagram.data(), datagram.size(), address, &port);

        if (datagram.size() < 1)
            return;

        int type = datagram[0];
        switch(type)
        {
            case 0x01:
                send_data.push_back(0x02);
                foreach(struct macro mac, macros)
                {
                    send_data.push_back(static_cast<char>(mac.code));
                    send_data.push_back(static_cast<char>(mac.description.size()));
                    for(int i = 0; i < mac.description.size(); i++)
                        send_data.push_back(static_cast<char>(mac.description.at(i).toLatin1()));
                }
                socket->writeDatagram(send_data, *address, port);
                break;
            case 0x03:
                if (datagram.size() < 2)
                    return;
                int code = datagram[1];
                foreach(struct macro mac, macros)
                {
                    if(mac.code == code)
                    {
                        press(mac.vector);
                        break;
                    }
                }
                break;
        }

    }
}

void NetThread::addMacro(struct macro mac)
{
    macros.push_back(mac);

    QFile file(QCoreApplication::applicationDirPath() + "/macros.qt");
    QDataStream stream(&file);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    stream << macros.count();
    foreach (struct macro mac, macros) {
        stream << mac.code << mac.description << mac.vector.count();
        foreach(struct keyEvent event, mac.vector)
        {
            stream << event.code << event.up;
        }
    }

    setNextCode();
    stream << next_code;
    file.close();

    emit updateTable();
}

void NetThread::delMacro(int index)
{
    next_code = macros[index].code;
    macros.remove(index);
    emit updateTable();

    QFile file(QCoreApplication::applicationDirPath() + "/macros.qt");
    QDataStream stream(&file);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    stream << macros.count();
    foreach (struct macro mac, macros) {
        stream << mac.code << mac.description << mac.vector.count();
        foreach(struct keyEvent event, mac.vector)
        {
            stream << event.code << event.up;
        }
    }
}

QVector<struct macro> NetThread::getVector()
{
    return macros;
}
