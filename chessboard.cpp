#include "chessboard.h"
#include "chessman.h"
#include "highlight.h"
#include "ai_thread.h"
#include "AI/aisample.h"
#include "AI/aialphabeta.h"
#include "AI/aigo.h"
#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include "constants.h"

Chessboard::Chessboard(QWidget *parent) : QWidget(parent), AIThinking(false)
{
    highlight = new Highlight(this);
    setFixedSize(TILE_SIZE * BOARD_SIZE, TILE_SIZE * BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            Chessman* &x = chessman[i][j];
            x = new Chessman(this);
            x->setGeometry(QRect(QPoint(i, j) * TILE_SIZE,
                                 QPoint(i + 1, j + 1) * TILE_SIZE));
            setChessboard(i, j, EMPTY);
            x->show();
        }
    gameStarted = false;
}

void Chessboard::setPlayer(PlayerType a, PlayerType b)
{
    player[0] = a;
    player[1] = b;
}

void Chessboard::rollback()
{
    if (AIThinking) return;
    if (history.size() < 2) return;
    for (int i = 0; i < 2; i++)
    {
        QPoint t = history.back();
        history.pop_back();
        setChessboard(t.rx(), t.ry(), EMPTY);
    }
    if (! history.empty())
        highlightTile(history.back().rx(), history.back().ry());
    else noHighlight();
}

void Chessboard::newGame()
{
    if (AIThinking) return;
    initGame();
    setPlayer(AI_alphabeta, AI_go);
//    setPlayer(AI_go, AI_alphabeta);
    //setPlayer(HUMAN, AI_alphabeta);
    //setPlayer(AI_alphabeta, HUMAN);
//    setPlayer(AI_sample, HUMAN);
    //setPlayer(AI_sample, AI_alphabeta);
    //setPlayer(AI_alphabeta, AI_alphabeta);
    gameStarted = true;
    startThinking();
}

void Chessboard::receiverResponse(const int &x, const int &y)
{
    processResponse(x, y);
}

void Chessboard::saveGame()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save"), ".", tr("History(*.txt)"));
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    QPoint item;
    foreach (item, history) {
        out << item.rx() << ' ' << item.ry() << endl;
    }
    file.close();
}

void Chessboard::loadGame()
{
    if (AIThinking) return;
    initGame();
    setPlayer(AI_alphabeta, HUMAN);
    //setPlayer(HUMAN, AI_alphabeta);

    QString path = QFileDialog::getOpenFileName(this, tr("Load"), ".", tr("History(*.txt)"));
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    int x, y;
    int gameisover = false;
    while (true)
    {
        in >> x >> y;
        if (in.atEnd()) break;
        if (placePiece(x, y))
        {
            gameisover = true;
            break;
        }
    }
    file.close();

    gameStarted = true;
    if (! gameisover) startThinking();
}

bool Chessboard::validPoint(const int &x, const int &y)
{
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool Chessboard::linkCheck(int sx, int sy, int dx, int dy)
{
    ChessType con = EMPTY;
    int z = 0;
    for (; validPoint(sx, sy); sx += dx, sy += dy)
    {
        ChessType &x = chessboard[sx][sy];
        if (x == con)
        {
            z++;
            if (z >= 5 && con != EMPTY) return true;
        }
        else
        {
            con = x;
            z = 1;
        }
    }
    return false;
}

bool Chessboard::gameOver()
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (linkCheck(i, 0, 0, 1)) return true;
        if (linkCheck(0, i, 1, 0)) return true;
        if (linkCheck(i, 0, 1, 1)) return true;
        if (linkCheck(0, i, 1, 1)) return true;
        if (linkCheck(i, 0, -1, 1)) return true;
        if (linkCheck(BOARD_SIZE - 1, i, -1, 1)) return true;
    }
    return false;
}

void Chessboard::setChessboard(const int &x, const int &y, const ChessType &t)
{
    chessboard[x][y] = t;
    chessman[x][y]->setType(t);
}

void Chessboard::highlightTile(const int &x, const int &y)
{
    highlight->setGeometry(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    highlight->showThis = true;
    highlight->update();
}

void Chessboard::noHighlight()
{
    highlight->showThis = false;
    highlight->update();
}

void Chessboard::initGame()
{
    QTime t = QTime::currentTime();
    srand(t.msec() + t.second() * 1000);
    currentPlayer = 0;
    player[0] = player[1] = HUMAN;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            setChessboard(i, j, EMPTY);

    noHighlight();
    history.clear();
}

void Chessboard::startThinking()
{
    AIThread* thread;
    switch (player[currentPlayer]) {
    case AI_sample:
        thread = new AISample(this);
        break;
    case AI_alphabeta:
        thread = new AIAlphaBeta(this);
        break;
    case AI_go:
        thread = new AIGO(this);
        break;
    case HUMAN:
    default:
        return;
        break;
    }
    Q_ASSERT(! AIThinking);
    AIThinking = true;
    thread->randomNumber = rand();
    thread->setChessboard((ChessType*) chessboard, currentPlayer);
    connect(thread, &AIThread::response,
            this, &Chessboard::receiverResponse);
    connect(thread, &AIThread::finished,
            thread, &QObject::deleteLater);
    thread->start();
}

bool Chessboard::placePiece(const int &x, const int &y)
{
    Q_ASSERT(chessboard[x][y] == EMPTY);
    if (currentPlayer == 0) setChessboard(x, y, BLACK);
    else if (currentPlayer == 1) setChessboard(x, y, WHITE);
    else Q_ASSERT(false);

    highlightTile(x, y);

    history.push_back(QPoint(x, y));
    currentPlayer ^= 1;
    return gameOver();
}

void Chessboard::processResponse(const int &x, const int &y)
{
    if (! validPoint(x, y) || chessboard[x][y] != EMPTY)
    {
        if (player[currentPlayer] != HUMAN) {
            qDebug() << x << ' ' << y;
            Q_ASSERT(false);
        }
        return;
    }
    if (player[currentPlayer] != HUMAN) {
        Q_ASSERT(AIThinking);
        AIThinking = false;
    }

    if (placePiece(x, y))
    {
        QMessageBox::information(this, tr("GameOver !"), "Game is Over!");
        return;
    }

    startThinking();
}

void Chessboard::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        painter.drawLine(TILE_SIZE / 2,
                         TILE_SIZE * i + TILE_SIZE / 2,
                         TILE_SIZE * BOARD_SIZE - TILE_SIZE / 2,
                         TILE_SIZE * i + TILE_SIZE / 2);
        painter.drawLine(TILE_SIZE * i + TILE_SIZE / 2,
                         TILE_SIZE / 2,
                         TILE_SIZE * i + TILE_SIZE / 2,
                         TILE_SIZE * BOARD_SIZE - TILE_SIZE / 2);
    }
}

void Chessboard::mousePressEvent(QMouseEvent *event)
{
    if (! gameStarted) return;
    if (player[currentPlayer] != HUMAN) return;
    QPoint pos = event->pos();
    processResponse(pos.x() / TILE_SIZE, pos.y() / TILE_SIZE);
}
