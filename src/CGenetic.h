#ifndef CGENETIC_H
#define CGENETIC_H

#include <QList>
#include "CFlappyGA.h"

class CGenetic {
public:
    CGenetic(int popSize, int startX, int startY, int ySol);
    ~CGenetic(void);

    QList<CFlappyGA *>& getPopulation(void);
    void nextGeneration(int startX, int startY, int ySol);
    int getGeneration(void) const;
    int getAllTimeBestScore(void) const;

private:
    int popSize;
    int generation;
    int allTimeBestScore;
    QList<CFlappyGA *> population;

    void triPopulation(void);
};

#endif // CGENETIC_H
