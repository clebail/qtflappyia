#include <string.h>
#include <math.h>
#include "CNeurone.h"

CNeurone::CNeurone(int nbGene) {
    this->nbGene = nbGene;
    genes = new CCapteur[nbGene];
    inputs = new double[nbGene - 1];
    initGenes();
    initSeuil();
}

CNeurone::~CNeurone(void) {
    delete[] genes;
    delete[] inputs;
}

void CNeurone::setInputs(double *inputs) {
    memcpy(this->inputs, inputs, (nbGene - 1) * sizeof(double));
}

double CNeurone::eval(double a) {
    double sigma = genes[0].getValue();
    for (int i = 1; i < nbGene; i++) {
        sigma += inputs[i - 1] * genes[i].getValue();
    }
    double eXp = exp(a * sigma);
    return eXp / (eXp + 1);
}

int CNeurone::getNbGene(void) {
    return nbGene;
}

void CNeurone::from(const CNeurone& n1, const CNeurone& n2, int seuil) {
    CNeurone *src = (CNeurone *)&n1;
    for (int i = 0; i < nbGene; i++) {
        if (i == seuil) src = (CNeurone *)&n2;
        genes[i].from(&src->genes[i]);
    }
    this->seuil = (rand() % 2 == 0) ? n1.seuil : n2.seuil;
}

void CNeurone::mute(int idx) {
    if (idx >= 0 && idx < nbGene) {
        genes[idx].init();
    }
    if (rand() % 2 == 0) {
        initSeuil();
    }
}

void CNeurone::initGenes(void) {
    for (int i = 0; i < nbGene; i++) {
        genes[i].init();
    }
}

void CNeurone::initSeuil(void) {
    seuil = ((double)(rand() % 801 + 100)) / 1000.0;
}

void CNeurone::copyFrom(const CNeurone& other) {
    seuil = other.seuil;
    for (int i = 0; i < nbGene; i++) {
        genes[i] = other.genes[i];
    }
}

double CNeurone::getSeuil(void) {
    return seuil;
}

void CNeurone::setSeuil(double seuil) {
    this->seuil = seuil;
}
