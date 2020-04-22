#include "mainwindow.h"
#include "addmacro.h"
#include "adddescription.h"
#include "viewmacros.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("AWPanel");
    this->setWindowIcon(QPixmap(QCoreApplication::applicationDirPath() + "/icon.png"));
    trayIcon = new QSystemTrayIcon(this);
    connect(this, SIGNAL(reset()), this, SLOT(cancel()));

    trayIcon->setIcon(QPixmap(QCoreApplication::applicationDirPath() + "/icon.png"));

    QMenu *menu = new QMenu(this);
    QAction *open = new QAction("Open", this);
    QAction *startup = new QAction("Run at startup", this);
    QAction *quit = new QAction("Quit", this);
    startup->setCheckable(true);

    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(settings.contains("AWPanel"))
        startup->setChecked(true);

    connect(open, SIGNAL(triggered()), this, SLOT(cancel()));
    connect(startup, SIGNAL(toggled (bool)), this, SLOT(startupChanged(bool)));
    connect(quit, SIGNAL(triggered()), this, SLOT(iconQuit()));

    menu->addAction(open);
    menu->addAction(startup);
    menu->addAction(quit);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    net.moveToThread(&thread);
    thread.start();

    setCentralMacros();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setCentralMacros()
{
    ViewMacros *view = new ViewMacros(this, &net);
    view->setMinimumSize(350, 200);
    setCentralWidget(view);
}

void MainWindow::addMacro()
{
    AddMacro *widget = new AddMacro(this);
    setCentralWidget(widget);
}

void MainWindow::cancel()
{
    setCentralMacros();

    if(!this->isVisible())
        this->show();
}

void MainWindow::next(QVector<struct keyEvent> vector)
{
    AddDescription *widget = new AddDescription(this, &net, vector);
    setCentralWidget(widget);
}

void MainWindow::startupChanged(bool state)
{
    if(state)
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.setValue("AWPanel", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        settings.sync();
    } else
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if(settings.contains("AWPanel"))
            settings.remove("AWPanel");
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(!quitFlag)
    {
        event->ignore();
        this->hide();
    } else
    {
        thread.quit();
        event->accept();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger && !this->isVisible())
        emit(cancel());
}

void MainWindow::iconQuit()
{
    quitFlag = 1;
    emit(close());
}
