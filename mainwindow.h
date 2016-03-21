#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Chessboard;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private:
    Chessboard* chessboard;
};

#endif // MAINWINDOW_H
