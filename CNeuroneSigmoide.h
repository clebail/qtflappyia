#ifndef CNEURONESIGMOIDE_H
#define CNEURONESIGMOIDE_H

#include "CNeurone.h"

class CNeuroneSigmoide : public CNeurone
{
public:
    CNeuroneSigmoide(int nbGene);
    virtual ~CNeuroneSigmoide(void);

    virtual double eval(double a);
};

#endif // CNEURONESIGMOIDE_H
