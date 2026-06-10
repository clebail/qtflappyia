#ifndef CNEURONE_H
#define CNEURONE_H

#include "CCapteur.h"

class CNeurone {
public:
    CNeurone(int nbGene);
    virtual ~CNeurone(void);

    void setInputs(double *inputs);
    virtual double eval(void) = 0;
    int getNbGene(void) const;

    void initGenes(void);
    void initGenesSmall(void);
    void backward(double delta, double eta, double *gradInputs = nullptr);
    double getGene(int idxGene) const;
    void setGene(int idxGene, double value);
protected:
    int nbGene;
    CCapteur *genes;
    double *inputs;
};

#endif // CNEURONE_H
