#include <QKeyEvent>
#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    timer=new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

    flappys << new Flappy(255, sceneWidget->getYSol()); //Fixme
    //flappys << new Flappy(200, sceneWidget->getYSol()); //Fixme
    //flappys << new Flappy(145, sceneWidget->getYSol()); //Fixme
    //flappys << new Flappy(90, sceneWidget->getYSol()); //Fixme
    sceneWidget->setFlappys(flappys);
    connect(sceneWidget, SIGNAL(ysolChange(int)), this, SLOT(onYsolChange(int)));

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow() {
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->key() == Qt::Key_Space) {
            for(int i=0;i<flappys.size();i++) {
                flappys[i]->up();
            }

            return true;
        }

    }

    return QObject::eventFilter(object, event);
}

void MainWindow::onTimer() {
}

void MainWindow::onYsolChange(int ySol) {
    qDebug() << ySol;

    for(int i=0;i<flappys.size();i++) {
        flappys[i]->setYSol(ySol);
    }
}
