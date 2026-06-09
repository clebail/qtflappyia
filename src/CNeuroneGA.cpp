#include <math.h>
#include "CNeuroneGA.h"

CNeuroneGA::CNeuroneGA(int nbGene) : CNeuroneSigmoide(nbGene) {
    initSeuil();
}

CNeuroneGA::~CNeuroneGA(void) {
}

void CNeuroneGA::from(const CNeuroneGA& n1, const CNeuroneGA& n2, int seuil) {
    CNeuroneGA *src = (CNeuroneGA *)&n1;
    for (int i = 0; i < nbGene; i++) {
        if (i == seuil) src = (CNeuroneGA *)&n2;
        genes[i].from(&src->genes[i]);
    }
    this->seuil = (rand() % 2 == 0) ? n1.seuil : n2.seuil;
}

void CNeuroneGA::mute(int idx) {
    if (idx >= 0 && idx < nbGene) {
        genes[idx].init();
    }
    if (rand() % 2 == 0) {
        initSeuil();
    }
}

void CNeuroneGA::initSeuil(void) {
    seuil = ((double)(rand() % 801 + 100)) / 1000.0;
}

void CNeuroneGA::copyFrom(const CNeuroneGA& other) {
    seuil = other.seuil;
    for (int i = 0; i < nbGene; i++) {
        genes[i] = other.genes[i];
    }
}

double CNeuroneGA::getSeuil(void) const {
    return seuil;
}

void CNeuroneGA::setSeuil(double seuil) {
    this->seuil = seuil;
}
