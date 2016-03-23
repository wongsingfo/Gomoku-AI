#include "aisample.h"
#include <QDebug>
#include <QVector>
#include <QPoint>
#include <QPair>

const int dx[] = {0, 1, 1, 1};
const int dy[] = {1, 0, 1, -1};
const int fx[] = {-1, -1, -1, 0, 0, 1, 1, 1, -2, 2, 0, 0};
const int fy[] = {-1, 0, 1, -1, 1, -1, 0, 1, 0, 0, -2, 2};
#define trans(p) ((p) == USER ? 0 : 1)

enum
{
    TWO,
    STWO,
    THREE,
    STHREE,
    FOUR,
    SFOUR,
    FIVE
};

#define check(x, y) ((x) >= 0 && (y) >= 0 && (x) < n && (y) < n)
#define checkn(x, y) ((x) < 0 || (y) < 0 || (x) >= n || (y) >= n)
#define USER 1
#define COMP -1
#define DEEP 2

AISample::AISample(QObject *parent) : AIThread(parent)
{

}

int AISample::dfs(int deep, int p)
{
    Count();

    int cur = trans(p);
    if (cnt[cur][FIVE]) return 10000;
    if (cnt[cur^1][FIVE]) return -10000;
    if (cnt[cur][FOUR] || cnt[cur][SFOUR]) return 10000;

    for (int i = 0; i < 2; i++)
    {
        int t = 0;
        if (cnt[i][FOUR]) t++;
        if (cnt[i][SFOUR]) t++;
        if (cnt[i][THREE]) t++;
        if (t >= 2)
        {
            if (i == cur) return 10000;
            else return - 10000;
        }
    }

    int *ccnt = (p == USER ? cnt[0] : cnt[1]);
    if (deep == 0) return
            ccnt[TWO] * 3 +
            ccnt[STWO] * 2 +
            ccnt[THREE] * 10 +
            ccnt[STHREE] * 5 +
            ccnt[SFOUR] * 10 +
            Evaluation(p);

    int ret = -8000;
    for (int i = 0; i < n; i ++)
        for (int j = 0; j < n; j ++)
            if (Exist(i, j))
            {
                board[i][j] = p;
                ret = qMax(ret, - dfs(deep-1, p == USER ? COMP : USER));
                board[i][j] = 0;
            }
    return ret;
}

int AISample::Evaluation(int p)
{
    int s1 = 0, s2 = 0;
    for (int i = 0; i < n; i ++)
        for (int j = 0; j < n; j ++)
            if (board[i][j])
                if (board[i][j] == p) s1 += PosV(i, j);
                else s2 += PosV(i, j);
            else {}
    return s1 - s2;
}

bool AISample::Exist(int i, int j)
{
    if (board[i][j]) return false;
    int x, y;
    for (int k = 0; k < 8; k ++)
    {
        x = i + fx[k];
        y = j + fy[k];
        if (check(x, y) && board[x][y]) return true;
    }
    return false;
}

void AISample::Count()
{
    memset(cnt, 0, sizeof(cnt));
    memset(flag, false, sizeof(flag));

    int s, x, y;
    for (int i = 0; i < n; i ++)
        for (int j = 0; j < n; j ++)
            if (board[i][j])
                for (int k = 0; k < 4; k ++)
                    if (flag[i][j][k] == false)
                    {
                        s = 1;
                        for (; ; s ++)
                        {
                            x = i + dx[k] * s;
                            y = j + dy[k] * s;
                            if (checkn(x, y) || board[x][y] != board[i][j]) break;
                            flag[x][y][k] = true;
                        }
                        if (s > 1)
                        {
                            int t = (int) empty(i - dx[k], j - dy[k]);
                            t += (int) empty(i + dx[k] * s, j + dy[k] * s);
                            int *ccnt = (board[i][j] == USER ? cnt[0] : cnt[1]);
                            if (t != 0)
                            {
                                if (s == 2) ccnt[t == 1 ? STWO : TWO] ++;
                                else if (s == 3) ccnt[t == 1 ? STHREE : THREE] ++;
                                else if (s == 4) ccnt[t == 1 ? SFOUR : FOUR] ++;
                            }
                            if (s == 5) ccnt[FIVE] ++;
                        }
                    }
}

bool cmp(const QPair<int, QPoint> &a, const QPair<int, QPoint> &b)
{
    return a.first < b.first;
}

void AISample::run()
{
    n = BOARD_SIZE;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            ChessType &x = chessboard[i][j];
            if (x == EMPTY) board[i][j] = 0;
            else if (x == BLACK) board[i][j] = COMP;
            else board[i][j] = USER;
        }

    QVector< QPair<int, QPoint> > sol;
    sol.push_back(QPair<int, QPoint>(-1000000,
                                     QPoint(BOARD_SIZE / 2,
                                            BOARD_SIZE / 2)));
    for (int i = 0; i < n; i ++)
        for (int j = 0; j < n; j ++)
            if (Exist(i, j))
            {
                board[i][j] = COMP;
                int tmp = - dfs(DEEP, USER);
                sol.push_back(QPair<int, QPoint>(tmp, QPoint(i, j)));
                board[i][j] = 0;
            }
    qSort(sol.begin(), sol.end(), cmp);
    while (sol.begin()->first + 3 < sol.back().first)
        sol.pop_front();
    int index = randomNumber % sol.size();
    //qDebug() << index << '/' << sol.size();
    qDebug() << sol[index].first;
    response(sol[index].second.rx(), sol[index].second.ry());
}

bool AISample::empty(int x, int y)
{
    return check(x, y) && board[x][y] == 0;
}

int AISample::PosV(int i, int j)
{
    return 10 - abs(i- (n >>1)) - abs(j- (n >>1));
}

#undef trans
#undef check
#undef checkn
#undef USER
#undef COMP
#undef DEEP
