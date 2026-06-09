#ifndef CNEURONELINEAIRE_H
#define CNEURONELINEAIRE_H

#include "CNeurone.h"

class CNeuroneLineaire : public CNeurone
{
public:
    CNeuroneLineaire(int nbGene);
    virtual ~CNeuroneLineaire(void);

    virtual double eval(void);
};

#endif // CNEURONELINEAIRE_H
