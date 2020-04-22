#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "netthread.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void iconQuit();
    void addMacro();
    void next(QVector<struct keyEvent>);
    void cancel();
    void startupChanged(bool);

private:
    NetThread net;
    QThread thread;
    bool quitFlag = 0;
    QSystemTrayIcon *trayIcon;
    void setCentralMacros();

signals:
    void reset();
};
#endif // MAINWINDOW_H
