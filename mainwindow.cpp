#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QList<Flappy>  flappys;
    setupUi(this);

    timer=new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

    flappys << Flappy(255); //Fixme
    flappys << Flappy(200); //Fixme
    flappys << Flappy(145); //Fixme
    flappys << Flappy(90); //Fixme
    sceneWidget->setFlappys(flappys);
}

MainWindow::~MainWindow() {
}

void MainWindow::onTimer() {
}
