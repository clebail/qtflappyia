#include "flappy.h"

Flappy::Flappy() {
    common = Common::getInstance();

    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();
}

Flappy::~Flappy() {
    timer->stop();
    delete timer;
}

QImage Flappy::getImage() const {
    return common->getSpriteImage(Common::estFlappy, idx);
}

void Flappy::onTimer() {
    idx = (idx + 1) % common->getNbSpriteImage(Common::estFlappy);
}
