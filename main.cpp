#include "mainwindow.h"
#include <QApplication>
#include <math.h>
#include "CMLP.h"

void checkGradient(void);

int main(int argc, char *argv[]){
	srand((unsigned) time(NULL));

    QApplication a(argc, argv);
    MainWindow w;

    // checkGradient();

    w.show();

    return a.exec();
}

void checkGradient(void) {
    CMLP *cmlp = new CMLP();
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

    // Avant toute perturbation :
    cmlp->forward(inputs);
    qDebug() << "Q_orig:" << cmlp->getQ(0);

    wOrig = cmlp->getNeuroneSortie(0)->getGene(0);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig + epsilon);
    cmlp->forward(inputs);
    Q = cmlp->getQ(0);
    lPlus = 0.5 * (Q - cible) * (Q - cible);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig);

    // Après setGene + forward pour lPlus :
    qDebug() << "gene0 après +eps:" << cmlp->getNeuroneSortie(0)->getGene(0)
           << "Q lPlus:" << cmlp->getQ(0);

    wOrig = cmlp->getNeuroneSortie(0)->getGene(0);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig - epsilon);
    cmlp->forward(inputs);
    Q = cmlp->getQ(0);
    lMinus = 0.5 * (Q - cible) * (Q - cible);
    cmlp->getNeuroneSortie(0)->setGene(0, wOrig);

    // Après setGene + forward pour lMinus :
    qDebug() << "gene0 après -eps:" << cmlp->getNeuroneSortie(0)->getGene(0)
           << "Q lMinus:" << cmlp->getQ(0);

    gradNum = (lPlus - lMinus) / (2 * epsilon);

    cmlp->forward(inputs);
    QOrig = cmlp->getQ(0);
    gradAnalytique = QOrig - cible;

    qDebug() << "gradNum:" << gradNum << "gradAnalytique:" << gradAnalytique
               << "diff:" << fabs(gradNum - gradAnalytique);

    cmlp->getNeuroneSortie(0)->setGene(0, wOrig + epsilon);
    cmlp->forward(inputs);
    Q = cmlp->getQ(0);
    qDebug() << "Q lPlus:" << Q << "wOrig+eps:" << wOrig + epsilon << "gene0 lu:" << cmlp->getNeuroneSortie(0)->getGene(0);

    delete cmlp;
}
