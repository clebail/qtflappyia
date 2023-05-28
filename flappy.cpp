#include "flappy.h"

Flappy::Flappy(int y) {
    this->y = y;
    this->inc = INC * (rand() % 2 == 1 ? 1 : -1);
    this->nbInc = rand() % MAX_INC;
    common = Common::getInstance();
	this->idx = rand() % common->getNbSpriteImage(Common::estFlappy);
}

Flappy::Flappy(const Flappy& other) : Flappy(other.y) {
    this->idx = other.idx;
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
    idx = (idx + 1) % common->getNbSpriteImage(Common::estFlappy);
    y += inc;

    if(nbInc++ == MAX_INC) {
        inc *= -1;
        nbInc = 0;
    }

}

int Flappy::getY() const {
    return y;
}
