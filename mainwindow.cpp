#include <QKeyEvent>
#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    int yT;
    setupUi(this);

    xSol = 0;

    timer=new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

    yT = calculYT();

    flappys << new Flappy(150, 255, sceneWidget->getYSol()); //Fixme
    sceneWidget->setFlappys(flappys);


    tuyaux << new Tuyau(Common::estTuyauBas, SCENE_WIDTH, yT);
    tuyaux << new Tuyau(Common::estTuyauHaut, SCENE_WIDTH, yT);
    sceneWidget->setTuyaux(tuyaux);

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

int MainWindow::calculYT() const {
    int offset = SCENE_HEIGHT - TUYAU_HEIGHT;
    int espace = SCENE_HEIGHT - SOL_HEIGHT;

    return (rand() % (espace - offset)) + offset;
}

void MainWindow::onTimer() {
    int yT = calculYT();
    bool allOk = true;

    xSol = (xSol + 1) % SOL_OFFSET;

    for(int i=0;i<flappys.size();i++) {
        allOk &= flappys[i]->next();
    }

    if(!allOk) {
        timer->stop();
    }

    if(tuyaux.size() == 2) {
        if(tuyaux[0]->getX() <= (SCENE_WIDTH / 2 - tuyaux[0]->getSize().width() / 2)) {
            tuyaux << new Tuyau(Common::estTuyauBas, SCENE_WIDTH, yT);
            tuyaux << new Tuyau(Common::estTuyauHaut, SCENE_WIDTH, yT);

            sceneWidget->setTuyaux(tuyaux);
        }
    }

    for(int i=0;i<tuyaux.size();i++) {
        int limite = -tuyaux[i]->getSize().width();

        if(tuyaux[i]->getX() > limite) {
            tuyaux[i]->next();
        } else {
            tuyaux[i]->setX(SCENE_WIDTH);
            tuyaux[i]->setY(yT);
        }
    }

    sceneWidget->setXSol(xSol);

    sceneWidget->repaint();
}

void MainWindow::onYsolChange(int ySol) {
    for(int i=0;i<flappys.size();i++) {
        flappys[i]->setYSol(ySol);
    }
}
