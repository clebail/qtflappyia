#ifndef CNEURONEGA_H
#define CNEURONEGA_H

#include "CNeuroneSigmoide.h"

class CNeuroneGA : public CNeuroneSigmoide
{
public:
    CNeuroneGA(int nbGene);
    virtual ~CNeuroneGA(void);

    void from(const CNeuroneGA& n1, const CNeuroneGA& n2, int seuil);
    void mute(int idx);

    void initSeuil(void);
    void copyFrom(const CNeuroneGA& other);
    double getSeuil(void) const;
    void setSeuil(double seuil);
protected:
    double seuil;
};

#endif // CNEURONEGA_H
