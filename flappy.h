#ifndef FLAPPY_H
#define FLAPPY_H

#include <QObject>
#include <QTimer>
#include "common.h"

#define MAX_INC 5
#define INC     2

class Flappy : QObject {
    Q_OBJECT

public:
    Flappy(int y);
    Flappy(const Flappy& other);
    ~Flappy();

    Flappy& operator=(const Flappy& other);

    QImage getImage() const;
    void next();
    int getY() const;
private:
    int idx;
    int y;
    int inc, nbInc;
    Common *common;
};

#endif // FLAPPY_H
