#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual bool eventFilter(QObject *object, QEvent *event);

private:
    QTimer *timer;
    QList<Flappy *> flappys;

private slots:
    void onTimer();
    void onYsolChange(int ySol);
};
#endif // MAINWINDOW_H
