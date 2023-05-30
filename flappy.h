#ifndef FLAPPY_H
#define FLAPPY_H

#include <QObject>
#include <QTimer>
#include "common.h"

#define MAX_INC         5
#define MAX_CYCLE_UP    30
#define INC             2
#define INC_UP          2
#define INC_DOWN        3
#define ANGLE_UP_MAX   -30
#define ANGLE_STEP_UP   1
#define ANGLE_STEP_DOWN 5
#define ANGLE_DOWN_MAX  90

class Flappy : QObject {
    Q_OBJECT

public:
    Flappy(int y, int ySol);
    Flappy(const Flappy& other);
    ~Flappy();

    Flappy& operator=(const Flappy& other);

    QImage getImage() const;
    void next();
    int getY() const;
    int getAngle() const;
    void up();
    void setYSol(int ySol);
private:
    int idNext;
    int idx;
    int y, ySol;
    int inc, nbInc, nbCycleUp;
    bool onUp, onDown;
    Common *common;
    int angle;
};

#endif // FLAPPY_H
