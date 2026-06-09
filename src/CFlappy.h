#ifndef CFLAPPY_H
#define CFLAPPY_H

#include <QObject>
#include "common.h"
#include "tuyau.h"

#define MAX_INC         5
#define MAX_CYCLE_UP    40
#define INC             2
#define INC_UP          1
#define INC_DOWN        2
#define ANGLE_UP_MAX   -30
#define ANGLE_STEP_UP   1
#define ANGLE_STEP_DOWN 5
#define ANGLE_DOWN_MAX  90

class CFlappy : QObject {
    Q_OBJECT

public:
    CFlappy(int x, int y, int ySol, Common::ESpriteType spriteType);
    virtual ~CFlappy(void);

    QImage getImage(void) const;
    bool next(void);
    int getX(void) const;
    int getY(void) const;
    int getAngle(void) const;
    void up(void);
    void setYSol(int ySol);
    int getScore(void) const;
    void incScore(void);
    QPoint getTop(void) const;
    QPoint getTopRight(void) const;
    QPoint getRight(void) const;
    QPoint getBotomRight(void) const;
    QPoint getBotom(void) const;
    QPoint getTopLeft(void) const;
    QPoint getBotomLeft(void) const;
    bool toucheUnTuyau(QList<Tuyau *> tuyaux) const;
    QList<QPair<QPoint, QPoint>> getSensors(QList<Tuyau *> tuyaux) const;

    virtual void think(QList<Tuyau *> tuyaux) = 0;
    void reset(int x, int y, int ySol);
    bool isDead(void) const;
    void markDead(void);
    double getVitesse(void) const;
protected:
    int inc, nbInc, nbCycleUp;
    int x, y, ySol;
    int idNext;
    int idx;
    bool onUp, onDown;
    int angle;
    int score;
    bool dead;
    int age;
    int nbBattements;
    Common *common;
    Common::ESpriteType spriteType;
private:
    QPoint raycast(QPoint start, double angleDeg, QList<Tuyau *> tuyaux) const;
};

#endif // CFLAPPY_H
