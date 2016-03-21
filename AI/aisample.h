#ifndef SAMPLE_H
#define SAMPLE_H

#include "ai_thread.h"


class AISample : public AIThread
{
public:
    AISample(QObject* parent = 0);
    void run();

private:
    int n;
    int board[BOARD_SIZE][BOARD_SIZE];
    int cnt[2][7];
    bool flag[BOARD_SIZE][BOARD_SIZE][4];

    bool empty(int x, int y);
    int PosV(int i, int j);
    int dfs(int, int);
    int Evaluation(int p);
    bool Exist(int i, int j);
    void Count();
};


#endif // SAMPLE_H
