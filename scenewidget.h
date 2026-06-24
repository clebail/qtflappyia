#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include "flappy.h"
#include "tuyau.h"

#define SOL_OFFSET      13

class SceneWidget : public QWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    void setFlappys(const QList<Flappy *>& flappys);
    void setTuyaux(const QList<Tuyau *>& tuyaux);
    int getYSol() const;
    int getXSol() const;
    void setXSol(int xSol);
    void setStats(int generation, int alive, int total, int bestScore, int curScore);
    ~SceneWidget();

public slots:
    void setShowSensors(bool show);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QImage fond;
    QImage sol;
    int xSol, ySol, xTuyau;
    bool showSensors;
    int statGeneration, statAlive, statTotal, statBestScore, statCurScore;
    QList<Flappy *> flappys;
    QList<Tuyau *> tuyaux;

    void drawStats(QPainter &painter);

signals:
    void ysolChange(int ySol);
};

#endif // SCENEWIDGET_H
