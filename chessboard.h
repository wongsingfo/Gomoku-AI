#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include "constants.h"

class Chessman;
class Highlight;

class Chessboard : public QWidget
{
    Q_OBJECT
public:
    explicit Chessboard(QWidget *parent = nullptr);

signals:

public slots:
    void newGame();
    void receiverResponse(const int &x, const int &y);
    void saveGame();
    void loadGame();
    void setPlayer(PlayerType, PlayerType);
    void rollback();

private:
    QVector<QPoint> history;

    bool validPoint(const int &x, const int &y);
    bool linkCheck(int sx, int sy, int dx, int dy);
    bool gameOver();

    bool AIThinking;
    bool gameStarted;
    int currentPlayer;
    PlayerType player[2];
    Highlight* highlight;
    Chessman* chessman[BOARD_SIZE][BOARD_SIZE];
    ChessType chessboard[BOARD_SIZE][BOARD_SIZE];
    void setChessboard(const int &x, const int &y, const ChessType &);
    void highlightTile(const int &x, const int &y);
    void noHighlight();

    void initGame();
    void startThinking();
    bool placePiece(const int &x, const int &y); // return true iff game is over
    void processResponse(const int &x, const int &y);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
};

#endif // CHESSBOARD_H
