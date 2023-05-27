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

    fond.rects.append(QRect(0, 0, 563, 510));

    sol.rects.append(QRect(569, 0, 563, 111));

    flappy.rects.append(QRect(688, 121, 35, 25));
    flappy.rects.append(QRect(729, 121, 35, 25));
    flappy.rects.append(QRect(771, 121, 35, 25));

    tuyauHaut.rects.append(QRect(568, 117, 322, 510));

    tuyauBas.rects.append(QRect(624, 117, 241, 510));

    sprites.insert(Common::estFond, fond);
    sprites.insert(Common::estSol, sol);
    sprites.insert(Common::estFlappy, flappy);
    sprites.insert(Common::estTuyauHaut, tuyauHaut);
    sprites.insert(Common::estTuyauBas, tuyauBas);
}

QImage Common::getSpriteImage(const ESpriteType& type, int idx) const {
    QList<QRect> rects = sprites.value(type).rects;

    if(idx < 0 || idx >= rects.size()) idx = 0;

    return spritesImage.copy(rects.at(idx));
}

int Common::getNbSpriteImage(const ESpriteType& type) const {
    return sprites.value(type).rects.size();
}
