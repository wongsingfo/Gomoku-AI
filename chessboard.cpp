#include "chessboard.h"
#include "chessman.h"
#include "highlight.h"
#include "ai_thread.h"
#include "AI/aisample.h"
#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include "constants.h"

Chessboard::Chessboard(QWidget *parent) : QWidget(parent)
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

void Chessboard::newGame()
{
    if (AIThinking) return;
    initGame();
    player[0] = AI_sample;
    player[1] = HUMAN;
    gameStarted = true;
    startThinking();
}

void Chessboard::receiverResponse(const int &x, const int &y)
{
    processResponse(x, y);
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

bool Chessboard::GameOver()
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

void Chessboard::initGame()
{
    currentPlayer = 0;
    player[0] = player[1] = HUMAN;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            setChessboard(i, j, EMPTY);

    highlight->showThis = false;
    highlight->update();
}

void Chessboard::startThinking()
{
    AIThread* thread;
    switch (player[currentPlayer]) {
    case AI_sample:
        thread = new AISample(this);
        break;
    case HUMAN:
    default:
        return;
        break;
    }
    Q_ASSERT(! AIThinking);
    AIThinking = true;
    thread->setChessboard((ChessType*) chessboard, currentPlayer);
    connect(thread, &AIThread::response,
            this, &Chessboard::receiverResponse);
    connect(thread, &AIThread::finished,
            thread, &QObject::deleteLater);
    thread->start();
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

    if (currentPlayer == 0) setChessboard(x, y, BLACK);
    else if (currentPlayer == 1) setChessboard(x, y, WHITE);
    else Q_ASSERT(false);

    highlight->setGeometry(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    highlight->showThis = true;
    highlight->update();

    if (GameOver())
    {
        QMessageBox::information(this, tr("GameOver!"), "Game is Over!");
        return;
    }

    currentPlayer ^= 1;
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
