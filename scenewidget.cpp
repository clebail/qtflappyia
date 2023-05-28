#include <QPainter>
#include "scenewidget.h"
#include "common.h"

SceneWidget::SceneWidget(QWidget *parent) : QWidget(parent) {
    Common *c = Common::getInstance();

    fond = c->getSpriteImage(Common::estFond);
    sol = c->getSpriteImage(Common::estSol);

    timerSol = new QTimer(this);
    timerSol->setInterval(10);
    connect(timerSol, SIGNAL(timeout()), this, SLOT(onTimerSol()));
    timerSol->start();

    xSol = 0;
}

void SceneWidget::setFlappys(const QList<Flappy>& flappys) {
    this->flappys = flappys;
}

SceneWidget::~SceneWidget() {
    timerSol->stop();
    delete timerSol;
}

void SceneWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(QPoint(0, 0), fond, QRect(0, 0, fond.width(), fond.height()));
    painter.drawImage(QRect(0, height()-sol.height(), width(), sol.height()), sol, QRect(xSol, 0, width(), sol.height()));

    for(int i=0;i<flappys.size();i++) {
        QImage img = flappys[i].getImage();
        painter.drawImage(QPoint(150, flappys[i].getY()), img, QRect(0, 0, img.width(), img.height()));
    }
}

void SceneWidget::onTimerSol() {
    static int idxFlappy = 0;

    xSol = (xSol + 1) % SOL_OFFSET;

    if(idxFlappy == 1) {
        for(int i=0;i<flappys.size();i++) {
            flappys[i].next();
        }
    }

    idxFlappy = (idxFlappy + 1) % 10;

    repaint();
}
