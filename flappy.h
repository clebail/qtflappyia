#ifndef FLAPPY_H
#define FLAPPY_H

#include <QObject>
#include <QTimer>
#include "common.h"
#include "tuyau.h"

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
    Flappy(int x, int y, int ySol);
    Flappy(const Flappy& other);
    ~Flappy();

    Flappy& operator=(const Flappy& other);

    QImage getImage() const;
    bool next();
    int getX() const;
    int getY() const;
    int getAngle() const;
    void up();
    void setYSol(int ySol);
    int getScore() const;
    void incScore();
    QPoint getTop() const;
    QPoint getTopRight() const;
    QPoint getRight() const;
    QPoint getBotomRight() const;
    QPoint getBotom() const;
    QList<QPair<QPoint, QPoint>> getSensors(QList<Tuyau *> tuyaux) const;
private:
    int idNext;
    int idx;
    int x, y, ySol;
    int inc, nbInc, nbCycleUp;
    bool onUp, onDown;
    Common *common;
    int angle;
    int score;
};

#endif // FLAPPY_H
