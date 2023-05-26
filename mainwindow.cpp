#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    label->setPixmap(QPixmap::fromImage(Common::getSpritesImage()));
}

MainWindow::~MainWindow() {
}

