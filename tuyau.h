#ifndef TUYAU_H
#define TUYAU_H

#include <QObject>
#include "common.h"

class Tuyau : public QObject {
    Q_OBJECT
public:
    Tuyau(const Common::ESpriteType& type, int x, int y);

    void next();
    int getY() const;
    void setY(int y);
    int getX() const;
    void setX(int x);
    QImage getImage() const;
    QSize getSize() const;
private:
    Common *common;
    Common::ESpriteType type;
    int x, y;
};

#endif // TUYAU_H
