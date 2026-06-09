#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include "CFlappyGA.h"
#include "tuyau.h"

#define SOL_OFFSET      13

class SceneWidget : public QWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    void setCFlappys(const QList<CFlappyGA *>& cflappys);
    void setTuyaux(const QList<Tuyau *>& tuyaux);
    int getYSol(void) const;
    int getXSol(void) const;
    void setXSol(int xSol);
    ~SceneWidget(void);

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
    QList<CFlappyGA *> cflappys;
    QList<Tuyau *> tuyaux;

signals:
    void ysolChange(int ySol);
};

#endif // SCENEWIDGET_H
