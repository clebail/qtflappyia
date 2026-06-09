#ifndef CFLAPPYGA_H
#define CFLAPPYGA_H

#include "CFlappy.h"
#include "CNeuroneGA.h"

class CFlappyGA : public CFlappy
{
public:
    CFlappyGA(int x, int y, int ySol);
    CFlappyGA(const CFlappyGA& other);
    ~CFlappyGA(void);

    CFlappyGA& operator=(const CFlappyGA& other);
    virtual void think(QList<Tuyau *> tuyaux);
    void from(CFlappyGA *f1, CFlappyGA *f2);
    int getFitness(void) const;
private:
    CNeuroneGA *neuronesCaches[FLAPPY_NB_HIDDEN];
    CNeuroneGA *neuroneSortie;
};

#endif // CFLAPPYGA_H
