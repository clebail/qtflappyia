#ifndef FLAPPY_H
#define FLAPPY_H

#include <QObject>
#include <QTimer>
#include "common.h"
#include "tuyau.h"
#include "CNeuroneGA.h"

#define MAX_INC         5
#define MAX_CYCLE_UP    40
#define INC             2
#define INC_UP          1
#define INC_DOWN        2
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
    QPoint getTopLeft() const;
    QPoint getBotomLeft() const;
    bool toucheUnTuyau(QList<Tuyau *> tuyaux) const;
    QList<QPair<QPoint, QPoint>> getSensors(QList<Tuyau *> tuyaux) const;

    void think(QList<Tuyau *> tuyaux);
    void from(Flappy *f1, Flappy *f2);
    void reset(int x, int y, int ySol);
    bool isDead() const;
    void markDead();
    int getFitness() const;

private:
    QPoint raycast(QPoint start, double angleDeg, QList<Tuyau *> tuyaux) const;

    int idNext;
    int idx;
    int x, y, ySol;
    int inc, nbInc, nbCycleUp;
    bool onUp, onDown;
    Common *common;
    int angle;
    int score;
    CNeuroneGA *neuronesCaches[FLAPPY_NB_HIDDEN];
    CNeuroneGA *neuroneSortie;
    bool dead;
    int age;
    int nbBattements;
};

#endif // FLAPPY_H
