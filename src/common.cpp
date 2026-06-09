#include <math.h>
#include "common.h"

static Common *instance = nullptr;

Common * Common::getInstance() {
    if(instance == nullptr) {
        instance = new Common();
    }

    return instance;
}

QImage Common::getSpritesImage() {
    return spritesImage;
}

Common::Common() {
    SSprite fond, sol, flappy, tuyauHaut, tuyauBas;

    spritesImage = QImage(":/images/flappy.png");

    fond.rects.append(QRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT));

    sol.rects.append(QRect(570, 0, 686, SOL_HEIGHT));

    flappy.rects.append(QRect(688, 121, FLAPPY_WIDTH, FLAPPY_HEIGHT));
    flappy.rects.append(QRect(729, 121, FLAPPY_WIDTH, FLAPPY_HEIGHT));
    flappy.rects.append(QRect(771, 121, FLAPPY_WIDTH, FLAPPY_HEIGHT));
    flappy.rects.append(QRect(729, 121, FLAPPY_WIDTH, FLAPPY_HEIGHT));

    tuyauHaut.rects.append(QRect(568, 117, 53, TUYAU_HEIGHT));

    tuyauBas.rects.append(QRect(624, 117, 53, TUYAU_HEIGHT));

    sprites.insert(Common::estFond, fond);
    sprites.insert(Common::estSol, sol);
    sprites.insert(Common::estFlappy, flappy);
    sprites.insert(Common::estTuyauHaut, tuyauHaut);
    sprites.insert(Common::estTuyauBas, tuyauBas);

    flappyHypo = sqrt(FLAPPY_HEIGHT2 * FLAPPY_HEIGHT2 + FLAPPY_WIDTH2 * FLAPPY_WIDTH2);
    flappyBaseAngle = asin(FLAPPY_HEIGHT2 / flappyHypo);
}

QImage Common::getSpriteImage(const ESpriteType& type, int idx) const {
    QList<QRect> rects = sprites.value(type).rects;

    if(idx < 0 || idx >= rects.size()) idx = 0;

    return spritesImage.copy(rects.at(idx));
}

int Common::getNbSpriteImage(const ESpriteType& type) const {
    return sprites.value(type).rects.size();
}

QSize Common::getSpriteSize(const ESpriteType& type) const {
    return sprites.value(type).rects[0].size();
}

float Common::getFlappyHypo() const {
    return flappyHypo;
}

float Common::getFlappyBaseAngle() const {
    return flappyBaseAngle;
}
