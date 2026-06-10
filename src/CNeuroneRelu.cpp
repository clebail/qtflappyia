#include <math.h>
#include "CNeuroneRelu.h"

CNeuroneRelu::CNeuroneRelu(int nbGene) : CNeurone(nbGene) {

}

CNeuroneRelu::~CNeuroneRelu(void) {
}

double CNeuroneRelu::eval(void) {
    double sigma = genes[0].getValue();
    for (int i = 1; i < nbGene; i++) {
        sigma += inputs[i - 1] * genes[i].getValue();
    }

    z = sigma;
    return sigma > 0 ? sigma : 0.0;
}

double CNeuroneRelu::getZ(void) const {
    return z;
}
