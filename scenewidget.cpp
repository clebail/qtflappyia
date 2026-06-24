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

    statGeneration = statAlive = statTotal = statBestScore = statCurScore = 0;
}

void SceneWidget::setStats(int generation, int alive, int total, int bestScore, int curScore) {
    statGeneration = generation;
    statAlive = alive;
    statTotal = total;
    statBestScore = bestScore;
    statCurScore = curScore;
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

void SceneWidget::setShowSensors(bool show) {
    showSensors = show;
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

    bool firstAlive = true;
    int ox = FLAPPY_WIDTH / 2;
    int oy = FLAPPY_HEIGHT / 2;

    for(int i=0;i<flappys.size();i++) {
        Flappy *f = flappys[i];
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

    drawStats(painter);
}

void SceneWidget::drawStats(QPainter &painter) {
    QStringList lignes;
    lignes << QString("Generation : %1").arg(statGeneration);
    lignes << QString("Vivants : %1 / %2").arg(statAlive).arg(statTotal);
    lignes << QString("Score actuel : %1").arg(statCurScore);
    lignes << QString("Meilleur score : %1").arg(statBestScore);

    QFont font = painter.font();
    font.setPixelSize(16);
    font.setBold(true);
    painter.setFont(font);

    int x = 10;
    int y = 24;
    int interligne = 22;

    for (int i = 0; i < lignes.size(); i++) {
        // Contour noir pour la lisibilite sur n'importe quel fond
        painter.setPen(QColorConstants::Black);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                painter.drawText(x + dx, y + i * interligne + dy, lignes[i]);
            }
        }
        painter.setPen(QColorConstants::White);
        painter.drawText(x, y + i * interligne, lignes[i]);
    }
}

void SceneWidget::resizeEvent(QResizeEvent *event) {
    ySol = event->size().height() - SOL_HEIGHT;
    emit(ysolChange(ySol));
}
