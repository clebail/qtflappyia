#include "flappy.h"

Flappy::Flappy(int y, int ySol) {
    common = Common::getInstance();

    this->y = y;
    this->ySol = ySol;
    this->inc = INC * (rand() % 2 == 1 ? 1 : -1);
    this->nbInc = rand() % MAX_INC;
	this->idx = rand() % common->getNbSpriteImage(Common::estFlappy);
    this->angle = 0;
    this->onUp = this->onDown = false;
    this->idNext = 0;
}

Flappy::Flappy(const Flappy& other) : Flappy(other.y, other.ySol) {
    this->inc = other.inc;
    this->nbInc = other.nbInc;
    this->idx = other.idx;
    this->angle = other.angle;
    this->onUp = other.onUp;
    this->onDown = other.onDown;
    this->idNext = other.idNext;
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

void Flappy::next() {
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
        y = qMin(y + INC_DOWN, ySol);
        angle = qMin(angle + ANGLE_STEP_DOWN, ANGLE_DOWN_MAX);
    }
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
