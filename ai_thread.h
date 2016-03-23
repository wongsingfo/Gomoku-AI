#ifndef AI_THREAD_H
#define AI_THREAD_H

#include <QThread>
#include "constants.h"

class AIThread : public QThread
{
    Q_OBJECT
public:
    explicit AIThread(QObject* parent = 0);

protected:
    ChessType chessboard[BOARD_SIZE][BOARD_SIZE];

public:
    int randomNumber;
    // always handle with black
    void setChessboard(ChessType *ptr, bool reverse);
    void run() Q_DECL_OVERRIDE;

signals:
    void response(const int &x, const int &y);
};

#endif // AI_THREAD_H
