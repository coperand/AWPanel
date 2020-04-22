#ifndef DELBUTTON_H
#define DELBUTTON_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>

class DelButton : public QPushButton
{
    Q_OBJECT
public:
    DelButton(QString text, int index);

private:
    int index;

private slots:
    void myClicked();

signals:
    void clickedIndex(int index);
};

#endif // DELBUTTON_H
