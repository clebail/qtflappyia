#include "mainwindow.h"
#include "common.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);
    xSol = 0;

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(sceneWidget, SIGNAL(ysolChange(int)), this, SLOT(onYsolChange(int)));
    connect(checkSensors, SIGNAL(toggled(bool)), sceneWidget, SLOT(setShowSensors(bool)));

    ga = new CGenetic(TAILLE_POPULATION, FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());
    sceneWidget->setFlappys(ga->getPopulation());

    resetTuyaux();
    timer->start();
}

MainWindow::~MainWindow() {
    delete ga;
    for (Tuyau *t : tuyaux) delete t;
}

int MainWindow::calculYT() const {
    int offset = TUYAU_GAP;
    int espace = SCENE_HEIGHT - SOL_HEIGHT;
    return (rand() % (espace - offset)) + offset;
}

void MainWindow::resetTuyaux() {
    for (Tuyau *t : tuyaux) delete t;
    tuyaux.clear();

    int yT = calculYT();
    tuyaux << new Tuyau(Common::estTuyauBas, SCENE_WIDTH, yT);
    tuyaux << new Tuyau(Common::estTuyauHaut, SCENE_WIDTH, yT);
    sceneWidget->setTuyaux(tuyaux);
}

void MainWindow::onTimer() {
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

    // Traitement de chaque piaf
    int aliveCount = 0;
    int bestCurrentScore = 0;
    QList<Flappy *>& pop = ga->getPopulation();

    for (Flappy *f : pop) {
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
            for (Flappy *f : pop) {
                if (!f->isDead()) f->incScore();
            }
        }
    }

    // Mise à jour des labels
    labelGen->setText(QString("Génération: %1").arg(ga->getGeneration()));
    labelAlive->setText(QString("Vivants: %1 / %2").arg(aliveCount).arg(TAILLE_POPULATION));
    labelBestScore->setText(QString("Meilleur score: %1").arg(ga->getAllTimeBestScore()));
    labelCurScore->setText(QString("Score actuel: %1").arg(bestCurrentScore));

    // Nouvelle génération si tout le monde est mort
    if (aliveCount == 0) {
        ga->nextGeneration(FLAPPY_START_X, FLAPPY_START_Y, sceneWidget->getYSol());
        sceneWidget->setFlappys(ga->getPopulation());
        resetTuyaux();
    }

    sceneWidget->setXSol(xSol);
    sceneWidget->repaint();
}

void MainWindow::onYsolChange(int ySol) {
    for (Flappy *f : ga->getPopulation()) {
        f->setYSol(ySol);
    }
}
