#include <math.h>
#include "flappy.h"

Flappy::Flappy(int x, int y, int ySol) {
    common = Common::getInstance();

    this->x = x;
    this->y = y;
    this->ySol = ySol;
    this->inc = INC * (rand() % 2 == 1 ? 1 : -1);
    this->nbInc = rand() % MAX_INC;
	this->idx = rand() % common->getNbSpriteImage(Common::estFlappy);
    this->angle = 0;
    this->onUp = this->onDown = false;
    this->idNext = 0;
    this->score = 0;
}

Flappy::Flappy(const Flappy& other) : Flappy(other.x, other.y, other.ySol) {
    this->inc = other.inc;
    this->nbInc = other.nbInc;
    this->idx = other.idx;
    this->angle = other.angle;
    this->onUp = other.onUp;
    this->onDown = other.onDown;
    this->idNext = other.idNext;
    this->score = other.score;
}

Flappy::~Flappy() {
}

Flappy& Flappy::operator=(const Flappy& other) {
    this->idx = other.idx;
    this->y = other.y;
    this->inc = other.inc;
    this->nbInc = other.nbInc;

    return *this;
}

QImage Flappy::getImage() const {
    QImage result = common->getSpriteImage(Common::estFlappy, idx);

    return result;
}

bool Flappy::next() {
    idNext = (idNext + 1) % 10;

    if(idNext == 1 && !onDown) {
        idx = (idx + 1) % common->getNbSpriteImage(Common::estFlappy);
        y += inc;

        if(nbInc++ == MAX_INC) {
            inc *= -1;
            nbInc = 0;
        }
    }

    if(onUp) {
        if(nbCycleUp++ == MAX_CYCLE_UP) {
            angle = 0;
            onUp = false;
            onDown = true;
        } else {
            y -= INC_UP;
            angle = qMax(angle - ANGLE_STEP_UP, ANGLE_UP_MAX);
        }
    } else if(onDown) {
        idx = 1;
        y += INC_DOWN;
        angle = qMin(angle + ANGLE_STEP_DOWN, ANGLE_DOWN_MAX);
    }

    return y >= 0 && y < ySol - FLAPPY_WIDTH + 2 * INC_DOWN;
}

int Flappy::getX() const {
    return x;
}

int Flappy::getY() const {
    return y;
}

int Flappy::getAngle() const {
    return angle;
}

void Flappy::up() {
    angle = 0;
    nbCycleUp = 0;
    onUp = true;
    onDown = false;
}

void Flappy::setYSol(int ySol) {
    this->ySol = ySol;
}

int Flappy::getScore() const {
    return score;
}

void Flappy::incScore() {
    score++;
}

QPoint Flappy::getTop() const {
    QPoint result;
    float topAngle = angle * PI / 180 - 90;

    result.setX(x + FLAPPY_WIDTH2 + cos(topAngle) * FLAPPY_HEIGHT2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(topAngle) * FLAPPY_HEIGHT2);

    return result;
}

QPoint Flappy::getTopRight() const {
    QPoint result;
    float topAngle = angle * PI / 180 - common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(topAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(topAngle) * common->getFlappyHypo());

    return result;
}

QPoint Flappy::getRight() const {
    QPoint result;

    result.setX(x + FLAPPY_WIDTH2 + cos(angle * PI / 180) * FLAPPY_WIDTH2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(angle * PI / 180) * FLAPPY_WIDTH2);

    return result;
}

QPoint Flappy::getBotomRight() const {
    QPoint result;
    float bottomAngle = angle * PI / 180 + common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(bottomAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(bottomAngle) * common->getFlappyHypo());


    return result;
}

QPoint Flappy::getBotom() const {
    QPoint result;
    float bottomAngle = angle * PI / 180 + 90;

    result.setX(x + FLAPPY_WIDTH2 + cos(bottomAngle) * FLAPPY_HEIGHT2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(bottomAngle) * FLAPPY_HEIGHT2);

    return result;
}

QList<QPair<QPoint, QPoint>> Flappy::getSensors(QList<Tuyau *> tuyaux) const {
    QList<QPair<QPoint, QPoint>> result;
    QPoint ptr = getTopRight();
    QPoint pr = getRight();
    QPoint pbr = getBotomRight();
    QPair<QPoint, QPoint> pairH;
    QPair<QPoint, QPoint> pairD;
    QPair<QPoint, QPoint> pairB;

    for(int i=0;i<tuyaux.size();i++) {
        Tuyau *t = tuyaux[i];
        QSize ts = t->getSize();

        if(t->getType() == Common::estTuyauHaut && ptr.x() >= t->getX() && ptr.x() <= t->getX() + ts.width() && ptr.y() >= t->getY() + ts.height()) {
            pairH = QPair<QPoint, QPoint>(ptr, QPoint(ptr.x(), t->getY() + ts.height()));
        }

        if(t->getType() == Common::estTuyauBas && pbr.x() >= t->getX() && pbr.x() <= t->getX() + ts.width() && pbr.y() <= t->getY()) {
            pairB = QPair<QPoint, QPoint>(pbr, QPoint(pbr.x(), t->getY()));
        }

        if(pairD.first.isNull() && pr.x() <= t->getX() && ((t->getType() == Common::estTuyauHaut && pr.y() <= t->getY() + ts.height()) || (t->getType() == Common::estTuyauBas && pr.y() >= t->getY()))) {
            pairD = QPair<QPoint, QPoint>(pr, QPoint(t->getX(), pr.y()));
        }
    }

    if(pairH.first.isNull()) {
        pairH = QPair<QPoint, QPoint>(ptr, QPoint(ptr.x(), 0));
    }

    if(pairB.first.isNull()) {
        pairB = QPair<QPoint, QPoint>(pbr, QPoint(pbr.x(), ySol));
    }

    if(pairD.first.isNull()) {
        pairD = QPair<QPoint, QPoint>(pr, QPoint(SCENE_WIDTH, pr.y()));
    }

    result.append(pairH);
    result.append(pairD);
    result.append(pairB);

    return result;
}
