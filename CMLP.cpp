#include "CMLP.h"

CMLP::CMLP(void) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i] = new CNeuroneRelu(FLAPPY_NB_INPUTS + 1);
    }

    for (int i = 0; i < NB_OUT; i++) {
        neuronesSortie[i] = new CNeuroneLineaire(FLAPPY_NB_HIDDEN + 1);
    }
}

CMLP::~CMLP(void) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        delete neuronesCaches[i];
    }

    for (int i = 0; i < NB_OUT; i++) {
        delete neuronesSortie[i];
    }

}

void CMLP::forward(double *inputs) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i]->setInputs(inputs);
        sortieCaches[i] = neuronesCaches[i]->eval();
    }

    for (int i = 0; i < NB_OUT; i++) {
        neuronesSortie[i]->setInputs(sortieCaches);
        sortie[i] = neuronesSortie[i]->eval();
    }
}

int CMLP::act(void) const {
    return sortie[0] >= sortie[1] ? 0 : 1;
}

void CMLP::backward(int action, double cible, double eta) {
    double gradInputs[FLAPPY_NB_HIDDEN];
    double delta = sortie[action] - cible;

    neuronesSortie[action]->backward(delta, eta, gradInputs);
}
