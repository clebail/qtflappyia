#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer=new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();
}

MainWindow::~MainWindow() {
}

void MainWindow::onTimer() {
}
