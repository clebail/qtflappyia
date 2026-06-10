#ifndef CFLAPPYRL_H
#define CFLAPPYRL_H

#include "CFlappy.h"
#include "CMLP.h"

class CFlappyRL : public CFlappy
{
public:
    typedef struct _STransition {
        double inputs[FLAPPY_NB_INPUTS];
        int action;
        double reward;
        double inputsNext[FLAPPY_NB_INPUTS];
        bool done;
    }STransition;

    CFlappyRL(int x, int y, int ySol);
    ~CFlappyRL(void);
    virtual void think(QList<Tuyau *> tuyaux);
    void afterStep(bool died, QList<Tuyau*> tuyaux);
    int getEpisode(void) const;
    double getEpsilon(void) const;
    int getBestScore(void) const;
private:
    CMLP *mlp, *mlpTarget;
    double epsilon;
    int episode;
    int bestScore;
    STransition buffer[RL_BUFFER_SIZE];
    int bufferHead, bufferSize;
    int stepCount;
    double lastInputs[FLAPPY_NB_INPUTS];
    int lastAction;
    int lastScore;

    int actEpsilonGreedy(void);
    double computeTarget(double* inputsNext, double reward, bool done);
    void learn(double* inputs, int action, double reward, double* inputsNext, bool done);
    void pushTransition(const STransition& transition);
    void maybeUpdateTarget(void);
};

#endif // CFLAPPYRL_H
