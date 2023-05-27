#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer=new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();
}

MainWindow::~MainWindow() {
}

void MainWindow::onTimer() {
    static int flappyIdx = 0;
    Common *c = Common::getInstance();

    label->setPixmap(QPixmap::fromImage(c->getSpriteImage(Common::estFlappy, flappyIdx)));

    flappyIdx = (flappyIdx + 1) % c->getNbSpriteImage(Common::estFlappy);
}
