#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPixmap>
#include "ui_mainwindow.h"
#include "CGenetic.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTimer *timer;
    CGenetic *ga;
    QList<Tuyau *> tuyaux;
    int xSol;
    int tickCount;
    int frameIndex;
    bool genInteressante;
    QList<QPixmap> frameBuffer;

    int calculYT() const;
    void resetTuyaux();
    void flushBuffer();

private slots:
    void onTimer();
    void onYsolChange(int ySol);
};

#endif // MAINWINDOW_H
