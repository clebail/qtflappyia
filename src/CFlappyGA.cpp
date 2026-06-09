#include <math.h>
#include "CFlappyGA.h"

CFlappyGA::CFlappyGA(int x, int y, int ySol) : CFlappy(x, y, ySol, Common::estFlappy) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i] = new CNeuroneGA(FLAPPY_NB_INPUTS + 1);
    }
    neuroneSortie = new CNeuroneGA(FLAPPY_NB_HIDDEN + 1);
}

CFlappyGA::CFlappyGA(const CFlappyGA& other) : CFlappyGA(other.x, other.y, other.ySol) {
    this->inc = other.inc;
    this->nbInc = other.nbInc;
    this->idx = other.idx;
    this->angle = other.angle;
    this->onUp = other.onUp;
    this->onDown = other.onDown;
    this->idNext = other.idNext;
    this->score = other.score;
    this->dead = other.dead;
    this->age = other.age;
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i]->copyFrom(*other.neuronesCaches[i]);
    }
    neuroneSortie->copyFrom(*other.neuroneSortie);
}

CFlappyGA::~CFlappyGA(void) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        delete neuronesCaches[i];
    }
    delete neuroneSortie;
}

CFlappyGA& CFlappyGA::operator=(const CFlappyGA& other) {
    this->idx = other.idx;
    this->y = other.y;
    this->inc = other.inc;
    this->nbInc = other.nbInc;

    return *this;
}

void CFlappyGA::think(QList<Tuyau *> tuyaux) {
    if (dead) return;

    QList<QPair<QPoint, QPoint>> sensors = getSensors(tuyaux);
    double inputs[FLAPPY_NB_INPUTS];
    double maxDist = sqrt((double)(SCENE_WIDTH * SCENE_WIDTH + SCENE_HEIGHT * SCENE_HEIGHT));
    for (int i = 0; i < FLAPPY_NB_INPUTS - 1; i++) {
        double dx = sensors[i].second.x() - sensors[i].first.x();
        double dy = sensors[i].second.y() - sensors[i].first.y();
        inputs[i] = sqrt(dx*dx + dy*dy) / maxDist;
    }
    inputs[FLAPPY_NB_INPUTS - 1] = getVitesse();

    // Couche cachée
    double hiddenOut[FLAPPY_NB_HIDDEN];
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i]->setInputs(inputs);
        hiddenOut[i] = neuronesCaches[i]->eval();
    }

    // Neurone de sortie
    neuroneSortie->setInputs(hiddenOut);
    if (neuroneSortie->eval() >= neuroneSortie->getSeuil() && !onUp) {
        up();
    }

    age++;
}

void CFlappyGA::from(CFlappyGA *f1, CFlappyGA *f2) {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        int s = rand() % neuronesCaches[i]->getNbGene();
        neuronesCaches[i]->from(*f1->neuronesCaches[i], *f2->neuronesCaches[i], s);
        if (rand() % 100 < TAUX_MUTATION) {
            neuronesCaches[i]->mute(rand() % neuronesCaches[i]->getNbGene());
        }
    }
    int s = rand() % neuroneSortie->getNbGene();
    neuroneSortie->from(*f1->neuroneSortie, *f2->neuroneSortie, s);
    if (rand() % 100 < TAUX_MUTATION) {
        neuroneSortie->mute(rand() % neuroneSortie->getNbGene());
    }
}

int CFlappyGA::getFitness(void) const {
    if (nbBattements == 0) return -1;
    return score * 100000 + age;
}

