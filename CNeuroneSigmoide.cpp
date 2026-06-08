#include <math.h>
#include "CNeuroneSigmoide.h"

CNeuroneSigmoide::CNeuroneSigmoide(int nbGene) : CNeurone(nbGene) {

}

CNeuroneSigmoide::~CNeuroneSigmoide(void) {
}

double CNeuroneSigmoide::eval(double a) {
    double sigma = genes[0].getValue();
    for (int i = 1; i < nbGene; i++) {
        sigma += inputs[i - 1] * genes[i].getValue();
    }

    double eXp = exp(a * sigma);
    return eXp / (eXp + 1);
}
