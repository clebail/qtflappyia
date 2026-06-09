#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include "CGenetic.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(void);

private:
    QTimer *timer;
    CGenetic *ga;
    QList<Tuyau *> tuyaux;
    int xSol;

    int calculYT(void) const;
    void resetTuyaux(void);

private slots:
    void onTimer(void);
    void onYsolChange(int ySol);
};

#endif // MAINWINDOW_H
