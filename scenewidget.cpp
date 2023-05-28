#include <QPainter>
#include "scenewidget.h"
#include "common.h"

SceneWidget::SceneWidget(QWidget *parent) : QWidget(parent) {
    Common *c = Common::getInstance();

    fond = c->getSpriteImage(Common::estFond);
    sol = c->getSpriteImage(Common::estSol);

    timerSol = new QTimer(this);
    timerSol->setInterval(20);
    connect(timerSol, SIGNAL(timeout()), this, SLOT(onTimerSol()));
    timerSol->start();

    xSol = 0;


}

SceneWidget::~SceneWidget() {
    timerSol->stop();
    delete timerSol;
}

void SceneWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(QPoint(0, 0), fond, QRect(0, 0, fond.width(), fond.height()));
    painter.drawImage(QRect(0, height()-sol.height(), width(), sol.height()), sol, QRect(xSol, 0, width(), sol.height()));
}

void SceneWidget::onTimerSol() {
    xSol = (xSol + 1) % SOL_OFFSET;

    repaint();
}
