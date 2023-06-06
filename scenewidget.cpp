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
}

void SceneWidget::setFlappys(const QList<Flappy *>& flappys) {
    this->flappys = flappys;
}

void SceneWidget::setTuyaux(const QList<Tuyau *>& tuyaux) {
    this->tuyaux = tuyaux;
}

int SceneWidget::getYSol() const {
    return ySol;
}

int SceneWidget::getXSol() const {
    return xSol;
}

void SceneWidget::setXSol(int xSol) {
    this->xSol = xSol;
}

SceneWidget::~SceneWidget() {
}

void SceneWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(QPoint(0, 0), fond, QRect(0, 0, fond.width(), fond.height()));

    for(int i=0;i<tuyaux.size();i++) {
        QImage img = tuyaux[i]->getImage();
        QSize size = tuyaux[i]->getSize();
        painter.drawImage(QPoint(tuyaux[i]->getX(), tuyaux[i]->getY()), img, QRect(0, 0, size.width(), TUYAU_HEIGHT));
    }

    for(int i=0;i<flappys.size();i++) {
        Flappy *f = flappys[i];
        QImage img = f->getImage();
        QList<QPair<QPoint, QPoint>> sensors = f->getSensors(tuyaux);

        int ox = FLAPPY_WIDTH / 2;
        int oy = FLAPPY_HEIGHT / 2;

        painter.save();
        painter.translate(QPoint(f->getX() + ox, f->getY() + oy));
        painter.rotate(f->getAngle());

        // painter.drawRect(QRect(-ox, -oy, FLAPPY_WIDTH, img.height()));
        painter.drawImage(QPoint(-ox, -oy), img, QRect(0, 0, FLAPPY_WIDTH, img.height()));
        painter.restore();

        painter.setPen(QColorConstants::Red);
        // painter.drawEllipse(f->getTop(), 5, 5);
        painter.drawEllipse(f->getTopRight(), 5, 5);
        painter.drawEllipse(f->getRight(), 5, 5);
        painter.drawEllipse(f->getBotomRight(), 5, 5);
        // painter.drawEllipse(f->getBotom(), 5, 5);

        for(int j=0;j<sensors.size();j++) {
            QPair<QPoint, QPoint> p = sensors[j];
            painter.drawLine(p.first, p.second);
        }
    }

    painter.drawImage(QRect(0, ySol, width(), SOL_HEIGHT), sol, QRect(xSol, 0, width(), SOL_HEIGHT));
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
    ySol = event->size().height() - SOL_HEIGHT;
    emit(ysolChange(ySol));
}
