#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    xSol = 0;

    timer = new QTimer(this);
    timer->setInterval(1);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(sceneWidget, SIGNAL(ysolChange(int)), this, SLOT(onYsolChange(int)));
    connect(checkSensors, SIGNAL(toggled(bool)), sceneWidget, SLOT(setShowSensors(bool)));

    ga = new CGenetic(TAILLE_POPULATION, FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());
    flappyRL = new CFlappyRL(FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());

    sceneWidget->setCFlappys(ga->getPopulation());
    sceneWidget->setCFlappyRL(flappyRL);

    resetTuyaux();
    timer->start();
}

MainWindow::~MainWindow(void) {
    delete ga;
    delete flappyRL;

    for (Tuyau *t : tuyaux) delete t;
}

int MainWindow::calculYT(void) const {
    int offset = TUYAU_GAP;
    int espace = SCENE_HEIGHT - SOL_HEIGHT;
    return (rand() % (espace - offset)) + offset;
}

void MainWindow::resetTuyaux(void) {
    for (Tuyau *t : tuyaux) delete t;
    tuyaux.clear();

    int yT = calculYT();
    tuyaux << new Tuyau(Common::estTuyauBas, SCENE_WIDTH, yT);
    tuyaux << new Tuyau(Common::estTuyauHaut, SCENE_WIDTH, yT);
    sceneWidget->setTuyaux(tuyaux);
}

void MainWindow::onTimer(void) {
    xSol = (xSol + 1) % SOL_OFFSET;
    int yT = calculYT();

    // Ajout d'une nouvelle paire de tuyaux quand la première atteint le milieu
    if (tuyaux.size() == 2) {
        if (tuyaux[0]->getX() <= SCENE_WIDTH / 2 - tuyaux[0]->getSize().width() / 2) {
            yT = calculYT();
            tuyaux << new Tuyau(Common::estTuyauBas, SCENE_WIDTH, yT);
            tuyaux << new Tuyau(Common::estTuyauHaut, SCENE_WIDTH, yT);
            sceneWidget->setTuyaux(tuyaux);
        }
    }

    // Déplacement des tuyaux
    for (Tuyau *t : tuyaux) {
        if (t->getX() > -t->getSize().width()) {
            t->next();
        } else {
            t->setX(SCENE_WIDTH);
            t->setY(yT);
        }
    }

    // Traitement de chaque piaf GA
    int aliveCount = 0;
    int bestCurrentScore = 0;
    QList<CFlappyGA *>& pop = ga->getPopulation();

    for (CFlappyGA *f : pop) {
        if (f->isDead()) continue;

        f->think(tuyaux);
        if (!f->next() || f->toucheUnTuyau(tuyaux)) {
            f->markDead();
        } else {
            aliveCount++;
            bestCurrentScore = qMax(bestCurrentScore, f->getScore());
        }
    }

    // Score : quand le bord droit d'un tuyau bas passe le bord gauche du piaf
    for (Tuyau *t : tuyaux) {
        if (t->getType() == Common::estTuyauBas && t->getX() + t->getSize().width() == FLAPPY_START_X) {
            for (CFlappyGA *f : pop) {
                if (!f->isDead()) f->incScore();
            }

            if(!flappyRL->isDead()) flappyRL->incScore();
        }
    }

    // Traitement du piaf RL
    if(!flappyRL->isDead()) {
        flappyRL->think(tuyaux);
        bool died = !flappyRL->next() || flappyRL->toucheUnTuyau(tuyaux);
        if (died) flappyRL->markDead();
        flappyRL->afterStep(died, tuyaux);
        //if (flappyRL->isDead()) {

        //    resetTuyaux();
        //}
    }

    // Mise à jour des labels
    labelGen->setText(QString("Génération: %1").arg(ga->getGeneration()));
    labelAlive->setText(QString("Vivants: %1 / %2").arg(aliveCount).arg(TAILLE_POPULATION));
    labelBestScore->setText(QString("Meilleur score: %1").arg(ga->getAllTimeBestScore()));
    labelCurScore->setText(QString("Score actuel: %1").arg(bestCurrentScore));
    labelRLEpisode->setText(QString("RL épisode: %1").arg(flappyRL->getEpisode()));
    labelRLScore->setText(QString("RL score: %1").arg(flappyRL->getScore()));
    labelRLBestScore->setText(QString("RL meilleur score: %1").arg(flappyRL->getBestScore()));
    labelRLEpsilon->setText(QString("RL ε: %1").arg(QString::number(flappyRL->getEpsilon(), 'f', 3)));

    // Nouvelle génération si tout le monde est mort
    if (aliveCount == 0 && flappyRL->isDead()) {
        ga->nextGeneration(FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());
        sceneWidget->setCFlappys(ga->getPopulation());
        flappyRL->reset(FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());
        resetTuyaux();
    }

    sceneWidget->setXSol(xSol);
    sceneWidget->repaint();
}

void MainWindow::onYsolChange(int ySol) {
    for (CFlappyGA *f : ga->getPopulation()) {
        f->setYSol(ySol);
    }
    flappyRL->setYSol(ySol);
}
