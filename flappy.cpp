#include <math.h>
#include "flappy.h"

Flappy::Flappy(int x, int y, int ySol) {
    common = Common::getInstance();

    this->x = x;
    this->y = y;
    this->ySol = ySol;
    this->inc = INC * (rand() % 2 == 1 ? 1 : -1);
    this->nbInc = rand() % MAX_INC;
    this->idx = rand() % common->getNbSpriteImage(Common::estFlappy);
    this->angle = 0;
    this->onUp = this->onDown = false;
    this->idNext = 0;
    this->nbCycleUp = 0;
    this->score = 0;
    this->dead = false;
    this->age = 0;
    this->nbBattements = 0;
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        neuronesCaches[i] = new CNeuroneGA(FLAPPY_NB_INPUTS + 1);
    }
    neuroneSortie = new CNeuroneGA(FLAPPY_NB_HIDDEN + 1);
}

Flappy::Flappy(const Flappy& other) : Flappy(other.x, other.y, other.ySol) {
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

Flappy::~Flappy() {
    for (int i = 0; i < FLAPPY_NB_HIDDEN; i++) {
        delete neuronesCaches[i];
    }
    delete neuroneSortie;
}

Flappy& Flappy::operator=(const Flappy& other) {
    this->idx = other.idx;
    this->y = other.y;
    this->inc = other.inc;
    this->nbInc = other.nbInc;

    return *this;
}

QImage Flappy::getImage() const {
    QImage result = common->getSpriteImage(Common::estFlappy, idx);

    return result;
}

bool Flappy::next() {
    idNext = (idNext + 1) % 10;

    if(idNext == 1 && !onDown) {
        idx = (idx + 1) % common->getNbSpriteImage(Common::estFlappy);
        y += inc;

        if(nbInc++ == MAX_INC) {
            inc *= -1;
            nbInc = 0;
        }
    }

    if(onUp) {
        if(nbCycleUp++ == MAX_CYCLE_UP) {
            angle = 0;
            onUp = false;
            onDown = true;
        } else {
            y -= INC_UP;
            angle = qMax(angle - ANGLE_STEP_UP, ANGLE_UP_MAX);
        }
    } else if(onDown) {
        idx = 1;
        y += INC_DOWN;
        angle = qMin(angle + ANGLE_STEP_DOWN, ANGLE_DOWN_MAX);
    }

    return y >= 0 && y < ySol - FLAPPY_WIDTH + 2 * INC_DOWN;
}

int Flappy::getX() const {
    return x;
}

int Flappy::getY() const {
    return y;
}

int Flappy::getAngle() const {
    return angle;
}

void Flappy::up() {
    angle = 0;
    nbCycleUp = 0;
    onUp = true;
    onDown = false;
    nbBattements++;
}

void Flappy::setYSol(int ySol) {
    this->ySol = ySol;
}

int Flappy::getScore() const {
    return score;
}

void Flappy::incScore() {
    score++;
}

QPoint Flappy::getTop() const {
    QPoint result;
    float topAngle = angle * PI / 180 - 90;

    result.setX(x + FLAPPY_WIDTH2 + cos(topAngle) * FLAPPY_HEIGHT2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(topAngle) * FLAPPY_HEIGHT2);

    return result;
}

QPoint Flappy::getTopRight() const {
    QPoint result;
    float topAngle = angle * PI / 180 - common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(topAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(topAngle) * common->getFlappyHypo());

    return result;
}

QPoint Flappy::getRight() const {
    QPoint result;

    result.setX(x + FLAPPY_WIDTH2 + cos(angle * PI / 180) * FLAPPY_WIDTH2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(angle * PI / 180) * FLAPPY_WIDTH2);

    return result;
}

QPoint Flappy::getBotomRight() const {
    QPoint result;
    float bottomAngle = angle * PI / 180 + common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(bottomAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(bottomAngle) * common->getFlappyHypo());


    return result;
}

QPoint Flappy::getBotom() const {
    QPoint result;
    float bottomAngle = angle * PI / 180 + 90;

    result.setX(x + FLAPPY_WIDTH2 + cos(bottomAngle) * FLAPPY_HEIGHT2);
    result.setY(y + FLAPPY_HEIGHT2 + sin(bottomAngle) * FLAPPY_HEIGHT2);

    return result;
}

QPoint Flappy::getTopLeft() const {
    QPoint result;
    float topAngle = angle * PI / 180 + PI + common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(topAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(topAngle) * common->getFlappyHypo());

    return result;
}

QPoint Flappy::getBotomLeft() const {
    QPoint result;
    float bottomAngle = angle * PI / 180 + PI - common->getFlappyBaseAngle();

    result.setX(x + FLAPPY_WIDTH2 + cos(bottomAngle) * common->getFlappyHypo());
    result.setY(y + FLAPPY_HEIGHT2 + sin(bottomAngle) * common->getFlappyHypo());

    return result;
}

void Flappy::think(QList<Tuyau *> tuyaux) {
    if (dead) return;

    QList<QPair<QPoint, QPoint>> sensors = getSensors(tuyaux);
    double inputs[FLAPPY_NB_INPUTS];
    double maxDist = sqrt((double)(SCENE_WIDTH * SCENE_WIDTH + SCENE_HEIGHT * SCENE_HEIGHT));
    for (int i = 0; i < FLAPPY_NB_INPUTS; i++) {
        double dx = sensors[i].second.x() - sensors[i].first.x();
        double dy = sensors[i].second.y() - sensors[i].first.y();
        inputs[i] = sqrt(dx*dx + dy*dy) / maxDist;
    }

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

void Flappy::from(Flappy *f1, Flappy *f2) {
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

void Flappy::reset(int x, int y, int ySol) {
    this->x = x;
    this->y = y;
    this->ySol = ySol;
    this->inc = INC * (rand() % 2 == 1 ? 1 : -1);
    this->nbInc = rand() % MAX_INC;
    this->idx = rand() % common->getNbSpriteImage(Common::estFlappy);
    this->angle = 0;
    this->onUp = this->onDown = false;
    this->idNext = 0;
    this->nbCycleUp = 0;
    this->score = 0;
    this->dead = false;
    this->age = 0;
    this->nbBattements = 0;
}

bool Flappy::isDead() const {
    return dead;
}

void Flappy::markDead() {
    dead = true;
}

int Flappy::getFitness() const {
    if (nbBattements == 0) return -1;
    return score * 100000 + age;
}

bool Flappy::toucheUnTuyau(QList<Tuyau *> tuyaux) const {
    const int m = 4;
    QRect birdRect(x + m, y + m, FLAPPY_WIDTH - 2*m, FLAPPY_HEIGHT - 2*m);
    for (Tuyau *t : tuyaux) {
        QSize ts = t->getSize();
        QRect pipeRect(t->getX(), t->getY(), ts.width(), TUYAU_HEIGHT);
        if (birdRect.intersects(pipeRect)) return true;
    }
    return false;
}

QPoint Flappy::raycast(QPoint start, double angleDeg, QList<Tuyau *> tuyaux) const {
    double rad = angleDeg * PI / 180.0;
    double dx = cos(rad);
    double dy = sin(rad);
    double minT = SCENE_WIDTH + SCENE_HEIGHT;
    double ex = start.x(), ey = start.y();

    // Bords de l'écran
    if (dy > 1e-6) {
        double t = (ySol - start.y()) / dy;
        if (t > 0 && t < minT) { minT = t; ex = start.x() + t*dx; ey = ySol; }
    } else if (dy < -1e-6) {
        double t = -start.y() / dy;
        if (t > 0 && t < minT) { minT = t; ex = start.x() + t*dx; ey = 0; }
    }
    if (dx > 1e-6) {
        double t = (SCENE_WIDTH - start.x()) / dx;
        if (t > 0 && t < minT) { minT = t; ex = SCENE_WIDTH; ey = start.y() + t*dy; }
    }

    // Intersection avec les tuyaux
    for (Tuyau *tp : tuyaux) {
        QSize ts = tp->getSize();
        int px = tp->getX(), pw = ts.width();

        if (tp->getType() == Common::estTuyauHaut) {
            int yBot = tp->getY() + TUYAU_HEIGHT;
            // Bord bas du tuyau haut
            if (qAbs(dy) > 1e-6) {
                double t = (yBot - start.y()) / dy;
                if (t > 0 && t < minT) {
                    double xh = start.x() + t*dx;
                    if (xh >= px && xh <= px + pw) { minT = t; ex = xh; ey = yBot; }
                }
            }
            // Bord gauche et droit
            if (qAbs(dx) > 1e-6) {
                for (int edge : {px, px + pw}) {
                    double t = (edge - start.x()) / dx;
                    if (t > 0 && t < minT) {
                        double yh = start.y() + t*dy;
                        if (yh >= tp->getY() && yh <= yBot) { minT = t; ex = edge; ey = yh; }
                    }
                }
            }
        }

        if (tp->getType() == Common::estTuyauBas) {
            int yTop = tp->getY();
            // Bord haut du tuyau bas
            if (qAbs(dy) > 1e-6) {
                double t = (yTop - start.y()) / dy;
                if (t > 0 && t < minT) {
                    double xh = start.x() + t*dx;
                    if (xh >= px && xh <= px + pw) { minT = t; ex = xh; ey = yTop; }
                }
            }
            // Bord gauche et droit
            if (qAbs(dx) > 1e-6) {
                for (int edge : {px, px + pw}) {
                    double t = (edge - start.x()) / dx;
                    if (t > 0 && t < minT) {
                        double yh = start.y() + t*dy;
                        if (yh >= yTop) { minT = t; ex = edge; ey = yh; }
                    }
                }
            }
        }
    }

    return QPoint((int)ex, (int)ey);
}

QList<QPair<QPoint, QPoint>> Flappy::getSensors(QList<Tuyau *> tuyaux) const {
    QList<QPair<QPoint, QPoint>> result;
    QPoint topRight = getTopRight();
    QPoint right    = getRight();
    QPoint botRight = getBotomRight();

    // FLAPPY_NB_FRONT rayons de -90° à +90° répartis équitablement (pas de 22.5°)
    // Les rayons vers le haut partent du coin supérieur droit,
    // vers le bas du coin inférieur droit, à l'horizontale du centre droit.
    // Ainsi le piaf « connaît » ses propres bords et ne sous-estime pas les collisions.
    for (int i = 0; i < FLAPPY_NB_FRONT; i++) {
        double angle = -90.0 + i * (180.0 / (FLAPPY_NB_FRONT - 1));
        QPoint origin = (angle < 0) ? topRight : (angle > 0) ? botRight : right;
        QPoint endpoint = raycast(origin, angle, tuyaux);
        result.append(QPair<QPoint, QPoint>(origin, endpoint));
    }

    // FLAPPY_NB_REAR rayons verticaux à l'arrière : depuis le coin arrière-haut
    // vers le haut (-90°) et le coin arrière-bas vers le bas (+90°). Ils détectent
    // les obstacles juste au-dessus et au-dessous de la queue, pour que le piaf ne
    // se cogne plus l'arrière contre un tuyau qu'il vient de dépasser de justesse.
    QPoint topLeft = getTopLeft();
    QPoint botLeft = getBotomLeft();
    result.append(QPair<QPoint, QPoint>(topLeft, raycast(topLeft, -90.0, tuyaux)));
    result.append(QPair<QPoint, QPoint>(botLeft, raycast(botLeft, 90.0, tuyaux)));

    return result;
}
