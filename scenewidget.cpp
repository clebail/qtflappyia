#include <QPainter>
#include <QResizeEvent>
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
    ySol = height()-sol.height();
}

void SceneWidget::setFlappys(const QList<Flappy *>& flappys) {
    this->flappys = flappys;
}

int SceneWidget::getYSol() const {
    return ySol;
}

SceneWidget::~SceneWidget() {
    timerSol->stop();
    delete timerSol;
}

void SceneWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(QPoint(0, 0), fond, QRect(0, 0, fond.width(), fond.height()));
    painter.drawImage(QRect(0, ySol, width(), sol.height()), sol, QRect(xSol, 0, width(), sol.height()));

    for(int i=0;i<flappys.size();i++) {
        QImage img = flappys[i]->getImage();
        int ox = img.width() / 2;
        int oy = img.height() / 2;

        painter.save();
        painter.translate(QPoint(150 + ox, flappys[i]->getY() + oy));
        painter.rotate(flappys[i]->getAngle());

        painter.drawImage(QPoint(-ox, -oy), img, QRect(0, 0, img.width(), img.height()));
        painter.restore();
    }
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
    ySol = event->size().height()-sol.height();
    emit(ysolChange(ySol));
}

void SceneWidget::onTimerSol() {
    xSol = (xSol + 1) % SOL_OFFSET;

    for(int i=0;i<flappys.size();i++) {
        flappys[i]->next();
    }

    repaint();
}
