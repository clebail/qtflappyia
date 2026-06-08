#ifndef CNEURONE_H
#define CNEURONE_H

#include "CCapteur.h"

class CNeurone {
public:
    CNeurone(int nbGene);
    virtual ~CNeurone(void);

    void setInputs(double *inputs);
    virtual double eval(void) = 0;
    int getNbGene(void);

    void initGenes(void);
protected:
    int nbGene;
    CCapteur *genes;
    double *inputs;
};

#endif // CNEURONE_H
