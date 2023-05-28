#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <QTimer>

#define SOL_OFFSET      13

class SceneWidget : public QWidget {
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QImage fond;
    QImage sol;
    QTimer *timerSol;
    int xSol;

private slots:
    void onTimerSol();
};

#endif // SCENEWIDGET_H
