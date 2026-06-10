#include <math.h>
#include "CFlappyRL.h"

CFlappyRL::CFlappyRL(int x, int y, int ySol) : CFlappy(x, y, ySol, Common::estFlappyRL) {
    mlp = new CMLP();
    mlpTarget = new CMLP();
    epsilon = RL_EPSILON_START;
    bufferHead = bufferSize = stepCount = episode = bestScore = lastScore = 0;
}

CFlappyRL::~CFlappyRL(void) {
    delete mlp;
    delete mlpTarget;
}

void CFlappyRL::think(QList<Tuyau *> tuyaux) {
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

    mlp->forward(inputs);
    memcpy(lastInputs, inputs, sizeof(lastInputs));
    lastAction = actEpsilonGreedy();

    if (lastAction && !onUp) {
        up();
    }

    age++;
}

int CFlappyRL::actEpsilonGreedy(void) {
    int result;

    if((double)rand() / RAND_MAX < epsilon) {
        result = rand() % 2;
    } else {
        result = mlp->act();
    }

    return result;
}

double CFlappyRL::computeTarget(double* inputsNext, double reward, bool done) {
    if(done) {
        return reward;
    }

    mlpTarget->forward(inputsNext);
    return reward + RL_GAMMA * qMax(mlpTarget->getQ(0), mlpTarget->getQ(1));
}

void CFlappyRL::learn(double* inputs, int action, double reward, double* inputsNext, bool done) {
    double y = computeTarget(inputsNext, reward, done);
    mlp->forward(inputs);
    mlp->backward(action, y, RL_ETA);
}

void CFlappyRL::pushTransition(const STransition& transition) {
    memcpy(&buffer[bufferHead], &transition, sizeof(STransition));
    bufferHead = (bufferHead+1) % RL_BUFFER_SIZE;
    bufferSize = qMin(bufferSize + 1, RL_BUFFER_SIZE);
}

int CFlappyRL::getEpisode(void) const {
    return episode;
}

double CFlappyRL::getEpsilon(void) const {
    return epsilon;
}

int CFlappyRL::getBestScore(void) const {
    return bestScore;
}

void CFlappyRL::maybeUpdateTarget(void) {
    if(stepCount % RL_TARGET_UPDATE == 0) {
        mlpTarget->copyGenesFromOther(mlp);
    }

    stepCount++;
}

void CFlappyRL::afterStep(bool died, QList<Tuyau*> tuyaux) {
    QList<QPair<QPoint, QPoint>> sensors = getSensors(tuyaux);
    double inputsNext[FLAPPY_NB_INPUTS];
    double maxDist = sqrt((double)(SCENE_WIDTH * SCENE_WIDTH + SCENE_HEIGHT * SCENE_HEIGHT));
    STransition tToPush;
    double reward;

    for (int i = 0; i < FLAPPY_NB_INPUTS - 1; i++) {
        double dx = sensors[i].second.x() - sensors[i].first.x();
        double dy = sensors[i].second.y() - sensors[i].first.y();
        inputsNext[i] = sqrt(dx*dx + dy*dy) / maxDist;
    }
    inputsNext[FLAPPY_NB_INPUTS - 1] = getVitesse();

    reward = died ? RL_REWARD_DEAD : RL_REWARD_TICK;
    if (score > lastScore) reward += RL_REWARD_PIPE;
    lastScore = score;

    memcpy(tToPush.inputs, lastInputs, sizeof(tToPush.inputs));
    tToPush.action = lastAction;
    tToPush.reward = reward;
    memcpy(tToPush.inputsNext, inputsNext, sizeof(tToPush.inputsNext));
    tToPush.done = died;

    pushTransition(tToPush);

    if (died) {
        bestScore = qMax(bestScore, score);
        episode++;
        lastScore = 0;
        epsilon = qMax(RL_EPSILON_MIN, epsilon * RL_EPSILON_DECAY);
    }

    if(bufferSize >= 1) {
        int batchSize = qMin(bufferSize, 32);
        for (int i = 0; i < batchSize; i++) {
            STransition& t = buffer[rand() % bufferSize];
            learn(t.inputs, t.action, t.reward, t.inputsNext, t.done);
        }
    }

    maybeUpdateTarget();
}
