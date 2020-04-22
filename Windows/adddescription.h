#ifndef ADDDESCRIPTION_H
#define ADDDESCRIPTION_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QVector>

#include "netthread.h"

Q_DECLARE_METATYPE(struct macro)

class AddDescription : public QWidget
{
    Q_OBJECT
public:
    AddDescription(QWidget *parent, NetThread *net, QVector<struct keyEvent> vector);

private:
    QVector<struct keyEvent> vector;
    QLabel *threshold;
    QTextEdit *descr;
    int code;

signals:
    void sendMacro(struct macro);
    void returnToMain();

public slots:
    void addClicked();
    void onTextChanged();
};

#endif // ADDDESCRIPTION_H
