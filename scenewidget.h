#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <QTimer>
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
    ~SceneWidget();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QImage fond;
    QImage sol;
    QTimer *timerSol;
    int xSol, ySol, xTuyau;
    QList<Flappy *> flappys;
    QList<Tuyau *> tuyaux;

private slots:
    void onTimerSol();

signals:
    void ysolChange(int ySol);
};

#endif // SCENEWIDGET_H
