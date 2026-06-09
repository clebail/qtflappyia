#ifndef CMLP_H
#define CMLP_H

#include "CNeuroneRelu.h"
#include "CNeuroneLineaire.h"
#include "common.h"

#define NB_OUT          2

class CMLP {
    friend class TestCMLP;
public:
    CMLP(void);
    ~CMLP(void);

    void forward(double *inputs);
    int act(void) const;
    void backward(int action, double cible, double eta);
    double getQ(int i) const;
private:
    CNeuroneRelu *neuronesCaches[FLAPPY_NB_HIDDEN];
    CNeuroneLineaire *neuronesSortie[NB_OUT];
    double sortieCaches[FLAPPY_NB_HIDDEN];
    double sortie[NB_OUT];

    double getSortieCache(int i) const;
    CNeuroneRelu* getNeuroneCache(int i) const;
    CNeuroneLineaire* getNeuroneSortie(int i) const;
};

#endif // CMLP_H
