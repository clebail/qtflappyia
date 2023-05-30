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

void SceneWidget::setTuyaux(const QList<Tuyau *>& tuyaux) {
    this->tuyaux = tuyaux;
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

    for(int i=0;i<tuyaux.size();i++) {
        QImage img = tuyaux[i]->getImage();
        QSize size = tuyaux[i]->getSize();
        painter.drawImage(QPoint(tuyaux[i]->getX(), tuyaux[i]->getY()), img, QRect(0, 0, size.width(), size.height()));
    }

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

    painter.drawImage(QRect(0, ySol, width(), sol.height()), sol, QRect(xSol, 0, width(), sol.height()));
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
    ySol = event->size().height()-sol.height();
    emit(ysolChange(ySol));
}

void SceneWidget::onTimerSol() {
    int yT =  (rand() % 279) + 121; //FIXME

    xSol = (xSol + 1) % SOL_OFFSET;

    for(int i=0;i<flappys.size();i++) {
        flappys[i]->next();
    }

    if(tuyaux.size() == 2) {
        if(tuyaux[0]->getX() <= (282 - tuyaux[0]->getSize().width() / 2)) {
            tuyaux << new Tuyau(Common::estTuyauBas, 563, yT);
            tuyaux << new Tuyau(Common::estTuyauHaut, 563, yT);
        }
    }

    for(int i=0;i<tuyaux.size();i++) {
        int limite = -tuyaux[i]->getSize().width();

        if(tuyaux[i]->getX() > limite) {
            tuyaux[i]->next();
        } else {
            tuyaux[i]->setX(width());
            tuyaux[i]->setY(yT);
        }
    }

    repaint();
}
