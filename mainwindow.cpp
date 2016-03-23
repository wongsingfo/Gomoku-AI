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

    QToolBar *toolBar = addToolBar(tr("Default"));

    QAction* newGameAction = new QAction(QIcon(":/icon/action/newGame"), tr("&New Game"), this);
    newGameAction->setShortcut(QKeySequence::New);
    newGameAction->setStatusTip("Create a new game.");
    connect(newGameAction, &QAction::triggered, chessboard, &Chessboard::newGame);
    toolBar->addAction(newGameAction);

    QAction* saveAction = new QAction(QIcon(":/icon/action/save"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save the game.");
    connect(saveAction, &QAction::triggered, chessboard, &Chessboard::saveGame);
    toolBar->addAction(saveAction);

    QAction* loadAction = new QAction(QIcon(":/icon/action/read"), tr("&Load"), this);
    loadAction->setShortcut(QKeySequence::Open);
    loadAction->setStatusTip("Load the game.");
    connect(loadAction, &QAction::triggered, chessboard, &Chessboard::loadGame);
    toolBar->addAction(loadAction);

    adjustSize();
    setWindowTitle(tr("Alpha Gomoku"));
    setWindowIcon(QIcon(":/icon/icon"));
    statusBar();
}

MainWindow::~MainWindow()
{

}

