#ifndef FLAPPY_H
#define FLAPPY_H

#include <QObject>
#include <QTimer>
#include "common.h"

class Flappy : QObject {
    Q_OBJECT

public:
    Flappy();
    ~Flappy();

    QImage getImage() const;
private:
    int idx;
    QTimer *timer;
    Common *common;

private slots:
    void onTimer();
};

#endif // FLAPPY_H
