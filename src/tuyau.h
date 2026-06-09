#ifndef TUYAU_H
#define TUYAU_H

#include <QObject>
#include "common.h"

class Tuyau : public QObject {
    Q_OBJECT
public:
    Tuyau(const Common::ESpriteType& type, int x, int y);

    void next(void);
    int getY(void) const;
    void setY(int y);
    int getX(void) const;
    void setX(int x);

    QImage getImage(void) const;
    QSize getSize(void) const;
    Common::ESpriteType getType(void) const;
private:
    Common *common;
    Common::ESpriteType type;
    int x, y;
};

#endif // TUYAU_H
