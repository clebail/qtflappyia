#include <QVector>
#include <math.h>
#include "TestCMLP.h"

void TestCMLP::initTestCase(void) {
    cmlp = new CMLP();
}

void TestCMLP::cleanupTestCase(void) {
    delete cmlp;
}

void TestCMLP::testGradient(void) {
    const double eps = 1e-4, eta = 1.0, cible = 1.0, tol = 1e-6;
    const int action = 0;
    double inputs[FLAPPY_NB_INPUTS];
    QVector<CNeurone *>neurones;
    int nbGenes = 0;
    int idxGene = 0;
    double *genes, *gradAna;

    for(int i=0;i<FLAPPY_NB_INPUTS;i++) {
        inputs[i] = 0.5;
    }

    nbGenes += cmlp->getNeuroneSortie(action)->getNbGene();
    neurones << cmlp->getNeuroneSortie(action);
    for(int i=0;i<FLAPPY_NB_HIDDEN;i++) {
        nbGenes += cmlp->getNeuroneCache(i)->getNbGene();
        neurones << cmlp->getNeuroneCache(i);
    }

    cmlp->forward(inputs);

    // Snapshot des poids
    idxGene = 0;
    genes = new double[nbGenes];
    gradAna = new double[nbGenes];
    for(int i=0;i<neurones.count();i++) {
        for(int j=0;j<neurones.at(i)->getNbGene();j++, idxGene++) {
            genes[idxGene] = neurones.at(i)->getGene(j);
        }
    }

    cmlp->backward(action, cible, eta);

    // Calcul du grad analytic
    idxGene = 0;
    for(int i=0;i<neurones.count();i++) {
        for(int j=0;j<neurones.at(i)->getNbGene();j++, idxGene++) {
            gradAna[idxGene] = (genes[idxGene] - neurones.at(i)->getGene(j)) / eta;
        }
    }

    // Restauration des poids
    idxGene = 0;
    for(int i=0;i<neurones.count();i++) {
        for(int j=0;j<neurones.at(i)->getNbGene();j++, idxGene++) {
            neurones.at(i)->setGene(j, genes[idxGene]);
        }
    }

    idxGene = 0;
    for(int i=0;i<neurones.count();i++) {
        for(int j=0;j<neurones.at(i)->getNbGene();j++, idxGene++) {
            double gene0 = neurones.at(i)->getGene(j);
            double lPlus, lMinus;
            double gradNum;
            double Q;

            neurones.at(i)->setGene(j, gene0 + eps);
            cmlp->forward(inputs);
            Q = cmlp->getQ(action);
            lPlus = 0.5 * (Q - cible) * (Q - cible);

            neurones.at(i)->setGene(j, gene0 - eps);
            cmlp->forward(inputs);
            Q = cmlp->getQ(action);
            lMinus = 0.5 * (Q - cible) * (Q - cible);

            neurones.at(i)->setGene(j, gene0);

            gradNum = (lPlus - lMinus) / (2 * eps);
            QVERIFY2(fabs(gradNum - gradAna[idxGene]) < tol, qPrintable(QString("idxGent=%1 num=%2 ana=%3 diff=%4").arg(idxGene).arg(gradNum).arg(gradAna[idxGene]).arg(fabs(gradNum - gradAna[idxGene]))));
        }
    }

    delete[] genes;
    delete[] gradAna;
}

QTEST_APPLESS_MAIN(TestCMLP)
