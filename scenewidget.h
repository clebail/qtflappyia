#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "flappy.h"

#define SOL_OFFSET      13

class SceneWidget : public QWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    void setFlappys(const QList<Flappy>& flappys);
    ~SceneWidget();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QImage fond;
    QImage sol;
    QTimer *timerSol;
    int xSol;
    QList<Flappy> flappys;

private slots:
    void onTimerSol();
};

#endif // SCENEWIDGET_H
