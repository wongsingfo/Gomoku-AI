#ifndef AIALPHABETA_H
#define AIALPHABETA_H

#include "ai_thread.h"

class AIAlphaBeta : public AIThread
{
public:
    AIAlphaBeta(QObject* parent = 0);
    void run();

private:
    int rx, ry; // best choice
    int board[BOARD_SIZE][BOARD_SIZE];
    int timer;
    int max_deep;
    bool exist(const int &x, const int &y);
    int dfs(int p, int alpha, int beta, int deep);
    int potential(const int &x, const int &y, const int &p);
    int potential2(const int &x, const int &y, const int &p);
    int evaluate();

    bool gameOver();
    bool linkCheck(int x, int y, int dx, int dy);
};

#endif // AIALPHABETA_H
