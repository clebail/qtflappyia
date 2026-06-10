#include "CMLP.h"

CMLP::CMLP(void) {
    for (int i = 0; i < RL_NB_HIDDEN; i++) {
        neuronesCaches[i] = new CNeuroneRelu(FLAPPY_NB_INPUTS + 1);
        neuronesCaches[i]->initGenesSmall();
    }

    for (int i = 0; i < NB_OUT; i++) {
        neuronesSortie[i] = new CNeuroneLineaire(RL_NB_HIDDEN + 1);
        neuronesSortie[i]->initGenesSmall();
    }
}

CMLP::~CMLP(void) {
    for (int i = 0; i < RL_NB_HIDDEN; i++) {
        delete neuronesCaches[i];
    }

    for (int i = 0; i < NB_OUT; i++) {
        delete neuronesSortie[i];
    }

}

void CMLP::forward(double *inputs) {
    for (int i = 0; i < RL_NB_HIDDEN; i++) {
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
    double gradInputs[RL_NB_HIDDEN];
    double delta = sortie[action] - cible;
    double erreurCaches[RL_NB_HIDDEN];

    neuronesSortie[action]->backward(delta, eta, gradInputs);

    for (int i = 0; i < RL_NB_HIDDEN; i++) {
        erreurCaches[i] = gradInputs[i] * (neuronesCaches[i]->getZ() > 0 ? 1.0 : 0.0);
        neuronesCaches[i]->backward(erreurCaches[i], eta);
    }
}

double CMLP::getQ(int i) const {
    if(i >= 0 && i < NB_OUT) {
        return sortie[i];
    }

    return 0;
}

double CMLP::getSortieCache(int i) const {
    if(i >= 0 && i < RL_NB_HIDDEN) {
        return sortieCaches[i];
    }

    return 0;
}

CNeuroneRelu * CMLP::getNeuroneCache(int i) const {
    if(i >= 0 && i < RL_NB_HIDDEN) {
        return neuronesCaches[i];
    }

    return nullptr;
}

CNeuroneLineaire * CMLP::getNeuroneSortie(int i) const {
    if(i >= 0 && i < NB_OUT) {
        return neuronesSortie[i];
    }

    return nullptr;
}

void CMLP::copyGenesFromOther(CMLP *other) {
    for(int i=0;i<RL_NB_HIDDEN;i++) {
        for(int j=0;j<other->getNeuroneCache(i)->getNbGene();j++) {
            getNeuroneCache(i)->setGene(j, other->getNeuroneCache(i)->getGene(j));
        }
    }

    for(int i=0;i<NB_OUT;i++) {
        for(int j=0;j<other->getNeuroneSortie(i)->getNbGene();j++) {
            getNeuroneSortie(i)->setGene(j, other->getNeuroneSortie(i)->getGene(j));
        }
    }
}
