#include <QPainter>
#include <QResizeEvent>
#include "scenewidget.h"
#include "common.h"

SceneWidget::SceneWidget(QWidget *parent) : QWidget(parent) {
    Common *c = Common::getInstance();

    fond = c->getSpriteImage(Common::estFond);
    sol = c->getSpriteImage(Common::estSol);

    xSol = 0;
    ySol = height() - SOL_HEIGHT;
    showSensors = false;
}

void SceneWidget::setCFlappys(const QList<CFlappyGA *>& cflappys) {
    this->cflappys = cflappys;
}

void SceneWidget::setTuyaux(const QList<Tuyau *>& tuyaux) {
    this->tuyaux = tuyaux;
}

int SceneWidget::getYSol(void) const {
    return ySol;
}

int SceneWidget::getXSol(void) const {
    return xSol;
}

void SceneWidget::setXSol(int xSol) {
    this->xSol = xSol;
}

void SceneWidget::setShowSensors(bool show) {
    showSensors = show;
}

SceneWidget::~SceneWidget(void) {
}

void SceneWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(QPoint(0, 0), fond, QRect(0, 0, fond.width(), fond.height()));

    for(int i=0;i<tuyaux.size();i++) {
        QImage img = tuyaux[i]->getImage();
        QSize size = tuyaux[i]->getSize();
        painter.drawImage(QPoint(tuyaux[i]->getX(), tuyaux[i]->getY()), img, QRect(0, 0, size.width(), TUYAU_HEIGHT));
    }

    bool firstAlive = true;
    int ox = FLAPPY_WIDTH / 2;
    int oy = FLAPPY_HEIGHT / 2;

    for(int i=0;i<cflappys.size();i++) {
        CFlappyGA *f = cflappys[i];
        if (f->isDead()) continue;

        QImage img = f->getImage();

        painter.save();
        painter.translate(QPoint(f->getX() + ox, f->getY() + oy));
        painter.rotate(f->getAngle());
        painter.drawImage(QPoint(-ox, -oy), img, QRect(0, 0, FLAPPY_WIDTH, img.height()));
        painter.restore();

        // Capteurs uniquement pour le premier piaf vivant, si activés
        if (firstAlive && showSensors) {
            firstAlive = false;
            QList<QPair<QPoint, QPoint>> sensors = f->getSensors(tuyaux);
            painter.setPen(QColorConstants::Red);
            painter.drawEllipse(f->getTopRight(), 4, 4);
            painter.drawEllipse(f->getRight(), 4, 4);
            painter.drawEllipse(f->getBotomRight(), 4, 4);
            painter.drawEllipse(f->getTopLeft(), 4, 4);
            painter.drawEllipse(f->getBotomLeft(), 4, 4);
            for(int j=0;j<sensors.size();j++) {
                QPair<QPoint, QPoint> p = sensors[j];
                painter.drawLine(p.first, p.second);
            }
        }
    }

    painter.drawImage(QRect(0, ySol, width(), SOL_HEIGHT), sol, QRect(xSol, 0, width(), SOL_HEIGHT));
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
    ySol = event->size().height() - SOL_HEIGHT;
    emit(ysolChange(ySol));
}
