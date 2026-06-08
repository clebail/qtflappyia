#include <string.h>
#include <math.h>
#include "CNeurone.h"

CNeurone::CNeurone(int nbGene) {
    this->nbGene = nbGene;
    genes = new CCapteur[nbGene];
    inputs = new double[nbGene - 1];
    initGenes();
}

CNeurone::~CNeurone(void) {
    delete[] genes;
    delete[] inputs;
}

void CNeurone::setInputs(double *inputs) {
    memcpy(this->inputs, inputs, (nbGene - 1) * sizeof(double));
}

int CNeurone::getNbGene(void) {
    return nbGene;
}

void CNeurone::initGenes(void) {
    for (int i = 0; i < nbGene; i++) {
        genes[i].init();
    }
}
