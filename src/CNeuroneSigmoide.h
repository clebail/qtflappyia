#ifndef CNEURONESIGMOIDE_H
#define CNEURONESIGMOIDE_H

#include "CNeurone.h"

class CNeuroneSigmoide : public CNeurone
{
public:
    CNeuroneSigmoide(int nbGene);
    virtual ~CNeuroneSigmoide(void);

    virtual double eval(void);
};

#endif // CNEURONESIGMOIDE_H
