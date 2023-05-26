#include "common.h"

static QImage spritesImage(":/images/flappy.png");

QImage Common::getSpritesImage() {
    return spritesImage;
}
