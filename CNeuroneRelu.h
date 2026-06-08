#ifndef CNEURONERELU_H
#define CNEURONERELU_H

#include "CNeurone.h"

class CNeuroneRelu : public CNeurone
{
public:
    CNeuroneRelu(int nbGene);
    virtual ~CNeuroneRelu(void);

    virtual double eval(void);
};

#endif // CNEURONERELU_H
