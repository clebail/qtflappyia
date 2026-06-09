#include <algorithm>
#include "CGenetic.h"
#include "common.h"

CGenetic::CGenetic(int popSize, int startX, int startY, int ySol)
    : popSize(popSize), generation(1), allTimeBestScore(0) {
    for (int i = 0; i < popSize; i++) {
        population << new Flappy(startX, startY, ySol);
    }
}

CGenetic::~CGenetic() {
    for (Flappy *f : population) delete f;
}

QList<Flappy *>& CGenetic::getPopulation() {
    return population;
}

void CGenetic::triPopulation() {
    std::sort(population.begin(), population.end(), [](Flappy *a, Flappy *b) {
        return a->getFitness() > b->getFitness();
    });
}

void CGenetic::nextGeneration(int startX, int startY, int ySol) {
    generation++;

    for (Flappy *f : population) {
        allTimeBestScore = qMax(allTimeBestScore, f->getScore());
    }

    triPopulation();

    int keepCount = qMax(2, popSize / 5);

    // Bottom 80%: crossover des meilleurs
    for (int i = keepCount; i < popSize; i++) {
        int i1 = rand() % keepCount;
        int i2 = rand() % keepCount;
        population[i]->from(population[i1], population[i2]);
    }

    // Tout le monde repart à zéro (position + état), neurone conservé
    for (Flappy *f : population) {
        f->reset(startX, startY, ySol);
    }
}

int CGenetic::getGeneration() const {
    return generation;
}

int CGenetic::getAllTimeBestScore() const {
    return allTimeBestScore;
}
