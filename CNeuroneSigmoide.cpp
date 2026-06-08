#include <math.h>
#include "CNeuroneSigmoide.h"
#include "common.h"

CNeuroneSigmoide::CNeuroneSigmoide(int nbGene) : CNeurone(nbGene) {

}

CNeuroneSigmoide::~CNeuroneSigmoide(void) {
}

double CNeuroneSigmoide::eval(void) {
    double sigma = genes[0].getValue();
    for (int i = 1; i < nbGene; i++) {
        sigma += inputs[i - 1] * genes[i].getValue();
    }

    double eXp = exp(PENTE_NEURONE * sigma);
    return eXp / (eXp + 1);
}
