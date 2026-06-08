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

void CNeurone::backward(double delta, double eta, double *gradInputs) {
    genes[0].setValue(genes[0].getValue() - eta * delta);
    for (int i = 1; i < nbGene; i++) {
        if(gradInputs != nullptr) {
            gradInputs[i - 1] = delta * genes[i].getValue();
        }
        genes[i].setValue(genes[i].getValue() - eta * delta * inputs[i - 1]);
    }
}

double CNeurone::getGene(int idxGene) const {
    if(idxGene >= 0 && idxGene < nbGene) {
        return genes[idxGene].getValue();
    }

    return 0;
}

void CNeurone::setGene(int idxGene, double value) {
    if(idxGene >= 0 && idxGene < nbGene) {
        genes[idxGene].setValue(value);
    }
}
