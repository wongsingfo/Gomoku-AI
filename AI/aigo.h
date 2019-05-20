#ifndef AIGO_H
#define AIGO_H

#include "ai_thread.h"

class AIGO : public AIThread
{
public:
    AIGO(QObject* parent = nullptr);
    virtual void run();

private:
    int rx, ry; // best choice
    int board[BOARD_SIZE][BOARD_SIZE];
    int timer;
    int max_deep;
    bool exist(const int &x, const int &y);
    int dfs(int p, int alpha, int beta, int depth);
    int potential(const int &x, const int &y, const int &p);
    int potential2(const int &x, const int &y, const int &p);
    QPoint fourAlive(const int &p);
    int evaluate();

    bool gameOver();
    bool linkCheck(int x, int y, int dx, int dy);
};

#endif // AIGO_H
