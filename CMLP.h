#ifndef CMLP_H
#define CMLP_H

#include "CNeuroneRelu.h"
#include "CNeuroneLineaire.h"
#include "common.h"

#define NB_OUT          2

class CMLP
{
public:
    CMLP(void);
    ~CMLP(void);

    void forward(double *inputs);
    int act(void) const;
    void backward(int action, double cible, double eta);
private:
    CNeuroneRelu *neuronesCaches[FLAPPY_NB_HIDDEN];
    CNeuroneLineaire *neuronesSortie[NB_OUT];
    double sortieCaches[FLAPPY_NB_HIDDEN];
    double sortie[NB_OUT];
};

#endif // CMLP_H
