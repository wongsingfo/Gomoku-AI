#include "mainwindow.h"
#include "chessboard.h"
#include <QAction>
#include <QToolBar>
#include <QBoxLayout>
#include <QWidget>
#include <QIcon>
#include "constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    chessboard = new Chessboard(this);
    setCentralWidget(chessboard);
    /*
    QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(formWidget);
      setLayout(layout);*/

    QAction* newGameAction = new QAction(QIcon(":/icon/action/newGame"), tr("&New Game"), this);
    newGameAction->setShortcut(QKeySequence::New);
    newGameAction->setStatusTip("Create a new game.");
    connect(newGameAction, &QAction::triggered, chessboard, &Chessboard::newGame);

    QToolBar *toolBar = addToolBar(tr("&New"));
    toolBar->addAction(newGameAction);

    adjustSize();
    setWindowTitle(tr("Alpha Gomoku"));
    setWindowIcon(QIcon(":/icon/icon"));
    statusBar();
}

MainWindow::~MainWindow()
{

}

