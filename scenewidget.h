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

    ~SceneWidget();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QImage fond;
    QImage sol;
    int xSol, ySol, xTuyau;
    QList<Flappy *> flappys;
    QList<Tuyau *> tuyaux;

signals:
    void ysolChange(int ySol);
};

#endif // SCENEWIDGET_H
