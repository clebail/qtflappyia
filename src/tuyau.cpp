#include "tuyau.h"

Tuyau::Tuyau(const Common::ESpriteType &type, int x, int y) {
    common = Common::getInstance();
    this->type = type;
    this->x = x;
    this->y = y;
    if(type == Common::estTuyauHaut) {
        this->y -= TUYAU_HEIGHT + TUYAU_GAP;
    }

}

void Tuyau::next(void) {
    x--;
}

int Tuyau::getY(void) const {
    return y;
}

void Tuyau::setY(int y) {
    this->y = y;
    if(type == Common::estTuyauHaut) {
        this->y -= TUYAU_HEIGHT + TUYAU_GAP;
    }
}

int Tuyau::getX(void) const {
    return x;
}

void Tuyau::setX(int x) {
    this->x = x;
}

QImage Tuyau::getImage(void) const {
    return common->getSpriteImage(type);
}

QSize Tuyau::getSize(void) const {
     return common->getSpriteSize(type);
}

Common::ESpriteType Tuyau::getType(void) const {
    return type;
}
