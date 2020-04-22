#ifndef ADDMACRO_H
#define ADDMACRO_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QVector>
#include <QSet>

#include "netthread.h"
#include "switch.h"

Q_DECLARE_METATYPE(QVector<struct keyEvent>)

class AddMacro : public QWidget
{
    Q_OBJECT
public:
    AddMacro(QWidget *parent);

protected:
   virtual void keyPressEvent(QKeyEvent *event);
   virtual void keyReleaseEvent(QKeyEvent *event);
   virtual void showEvent ( QShowEvent * event );

private:
    bool filled;
    int translateKey(int key);
    QVector<struct keyEvent> vector;
    QSet<unsigned short> pressed;
    QLabel *outputLabel;
    QLabel *threshold;
    Switch *ctrl;
    Switch *win;
    Switch *alt;

signals:
    void sendVector(QVector<struct keyEvent>);

public slots:
    void nextClicked();
    void switchChanged(int);
};

#endif // ADDMACRO_H
