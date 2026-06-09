#include <math.h>
#include "TestCMLP.h"

void TestCMLP::initTestCase(void) {
    cmlp = new CMLP();
}

void TestCMLP::cleanupTestCase(void) {
    delete cmlp;
}

void TestCMLP::testGradient(void) {
    double inputs[FLAPPY_NB_INPUTS];
    double wOrig;
    double epsilon = 0.0001;
    double Q, QOrig;
    double lPlus, lMinus;
    double gradNum, gradAnalytique;
    double cible = 1;

    for(int i=0;i<FLAPPY_NB_INPUTS;i++) {
        inputs[i] = 0.5;
    }
    cmlp->forward(inputs);

    wOrig = cmlp->getNeuroneSortie(0)->getGene(0);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig + epsilon);
    cmlp->forward(inputs);
    Q = cmlp->getQ(0);
    lPlus = 0.5 * (Q - cible) * (Q - cible);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig);

    wOrig = cmlp->getNeuroneSortie(0)->getGene(0);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig - epsilon);
    cmlp->forward(inputs);
    Q = cmlp->getQ(0);
    lMinus = 0.5 * (Q - cible) * (Q - cible);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig);

    gradNum = (lPlus - lMinus) / (2 * epsilon);

    cmlp->forward(inputs);
    QOrig = cmlp->getQ(0);
    gradAnalytique = QOrig - cible;

    QVERIFY(fabs(gradNum - gradAnalytique) < 0.000001);
}

QTEST_APPLESS_MAIN(TestCMLP)
