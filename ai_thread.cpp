#include "ai_thread.h"

AIThread::AIThread(QObject *parent) : QThread(parent)
{
}

void AIThread::setChessboard(ChessType* ptr, bool reverse)
{
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            chessboard[i][j] = *ptr;
            if (reverse) {
                if (*ptr == BLACK) chessboard[i][j] = WHITE;
                else if (*ptr == WHITE) chessboard[i][j] = BLACK;
            }

            ptr++;
        }
}

void AIThread::run()
{
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (chessboard[i][j] == EMPTY)
            {
                emit response(i, j);
                return;
            }
}
