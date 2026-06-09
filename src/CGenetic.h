#ifndef CGENETIC_H
#define CGENETIC_H

#include <QList>
#include "flappy.h"

class CGenetic {
public:
    CGenetic(int popSize, int startX, int startY, int ySol);
    ~CGenetic();

    QList<Flappy *>& getPopulation();
    void nextGeneration(int startX, int startY, int ySol);
    int getGeneration() const;
    int getAllTimeBestScore() const;

private:
    int popSize;
    int generation;
    int allTimeBestScore;
    QList<Flappy *> population;

    void triPopulation();
};

#endif // CGENETIC_H
