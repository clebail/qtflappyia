#ifndef CNEURONE_H
#define CNEURONE_H

#include "CCapteur.h"

class CNeurone {
public:
    CNeurone(int nbGene);
    virtual ~CNeurone(void);

    void setInputs(double *inputs);
    double eval(double a);
    int getNbGene(void);

    virtual void from(const CNeurone& n1, const CNeurone& n2, int seuil);
    virtual void mute(int idx);
    void initGenes(void);
    void initSeuil(void);
    void copyFrom(const CNeurone& other);

    double getSeuil(void);
    void setSeuil(double seuil);

protected:
    int nbGene;
    CCapteur *genes;
    double *inputs;
    double seuil;
};

#endif // CNEURONE_H
