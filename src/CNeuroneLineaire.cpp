#include <math.h>
#include "CNeuroneLineaire.h"

CNeuroneLineaire::CNeuroneLineaire(int nbGene) : CNeurone(nbGene) {

}

CNeuroneLineaire::~CNeuroneLineaire(void) {
}

double CNeuroneLineaire::eval(void) {
    double sigma = genes[0].getValue();
    for (int i = 1; i < nbGene; i++) {
        sigma += inputs[i - 1] * genes[i].getValue();
    }

    return sigma;
}
