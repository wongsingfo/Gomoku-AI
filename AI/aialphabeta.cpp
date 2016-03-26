#include "aialphabeta.h"
#include <QVector>
#include <QPoint>
#include <QDebug>
#include "constants.h"

#define n BOARD_SIZE
#define INF 1000000
#define timelimit 1000
#define check(x, y) ((x) >= 0 && (x) < n && (y) >= 0 && (y) < n)

struct Choice
{
    Choice (int _score = 0, int _x = 0, int _y = 0)
        : score(_score), x(_x), y(_y)
    {
    }

    int score;
    int x, y;
    bool operator < (const Choice &o) const
    {
        return score > o.score;
    }
};

AIAlphaBeta::AIAlphaBeta(QObject *parent) : AIThread(parent)
{

}

void AIAlphaBeta::run()
{
    timer = 0;
    rx = n / 2, ry = n / 2;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (chessboard[i][j] == BLACK) board[i][j] = 0;
            else if (chessboard[i][j] == WHITE) board[i][j] = 1;
            else board[i][j] = -1;
        }

    for (max_deep = 0; timer < timelimit; max_deep++)
    {
        int tmp = dfs(0, -INF - 233, INF + 233, max_deep);
        qDebug() << "depth:" << max_deep << "expectance:" << tmp;
        if (tmp <= -INF || tmp >= INF) break;
    }
    qDebug() << timer << "phases have been visited.";
    response(rx, ry);
}

bool AIAlphaBeta::exist(const int &x, const int &y)
{
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int p = x + dx, q = y + dy;
            if (check(p, q) && chessboard[p][q] != -1) return true;
        }
    return false;
}

int AIAlphaBeta::dfs(int p, int alpha, int beta, int depth)
{
    if (gameOver()) return -INF;
    if (depth < 0) return evaluate() * (p ? -1 : 1);
    timer++;

    bool firstLayer = depth == max_deep;
    QPoint tmpPoint;
    tmpPoint = fourAlive(p);
    if (tmpPoint.rx() != -1)
    {
        if (firstLayer)
        {
            rx = tmpPoint.rx();
            ry = tmpPoint.ry();
        }
        return INF;
    }
    tmpPoint = fourAlive(p ^ 1);
    if (tmpPoint.rx() != -1)
    {
        if (firstLayer)
        {
            rx = tmpPoint.rx();
            ry = tmpPoint.ry();
            return -INF;
        }
        board[tmpPoint.rx()][tmpPoint.ry()] = p;
        int ret = - dfs(p ^ 1, -beta, -alpha, depth);
        board[tmpPoint.rx()][tmpPoint.ry()] = -1;
        return ret;
    }

    QVector<Choice> que;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (board[i][j] != -1) continue;
            if (! exist(i, j)) continue;
            que.push_back(Choice(
                              //potential(i, j, p) + potential(i, j, p ^ 1),
                              //qMax(potential(i, j, p), potential(i, j, p ^ 1)),
                              qMax(potential(i, j, p), potential(i, j, p ^ 1)) * 2 +
                              qMax(potential2(i, j, p), potential2(i, j, p ^ 1)),
                                 i, j));
        }
    qSort(que);
    while (que.size() > 10)
        que.pop_back();

    Choice c;

    /*foreach (c, que) {
        qDebug() << c.x << ' ' << c.y << ' ' << c.score;
    }*/

    foreach (c, que) {
        board[c.x][c.y] = p;
        int tmp = -dfs(p ^ 1, -beta, -alpha, depth - 1);
        board[c.x][c.y] = -1;
        if (tmp >= beta)
            return beta;
        if (tmp > alpha)
        {
            alpha = tmp;
            if (firstLayer) rx = c.x, ry = c.y;
        }
    }
    return alpha;
}

int AIAlphaBeta::potential(const int &x, const int &y, const int &p)
{
    int ret = 0;
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        {
            if (! dx && ! dy) continue;
            int tmp = 0;
            int c = 0;
            for (int s = 1, px = x, py = y; s < 5; s++, px += dx, py += dy)
            {
                if (! check(px, py)) break;
                if (board[px][py] == (p ^ 1))
                {
                    tmp = 0;
                    break;
                }
                if (board[px][py] == -1) tmp += 1;
                else  // board[px][py] == p
                {
                    c++;
                    tmp += (6 - s) * (6 - s);
                }
            }
            ret += tmp;
        }
    return ret;
}

const int d4[][2] = {
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
};

int AIAlphaBeta::potential2(const int &x, const int &y, const int &p)
{
    int ret = 0;
    int dx, dy;
    for (int d = 0; d < 4; d++)
    {
        int c = 0, cs = 1;
        dx = d4[d][0], dy = d4[d][1];
        for (int s = 1, px = x + dx, py = y + dy; s < 5; s++, px += dx, py += dy)
        {
            if (! check(px, py)) break;
            if (board[px][py] == (p ^ 1)) break;
            cs++;
            if (board[px][py] == p) c++;
        }
        dx = -dx, dy = -dy;
        for (int s = 1, px = x + dx, py = y + dy; s < 5; s++, px += dx, py += dy)
        {
            if (! check(px, py)) break;
            if (board[px][py] == (p ^ 1)) break;
            cs++;
            if (board[px][py] == p) c++;
        }
        if (cs >= 5) ret += c * c * c;
    }
    return ret;
}

QPoint AIAlphaBeta::fourAlive(const int &p)
{
    for (int d = 0; d < 4; d++)
    {
        int dx = d4[d][0];
        int dy = d4[d][1];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                if (board[i][j] != -1) continue;

                int c = 0;
                for (int x = i + dx, y = j + dy; check(x, y) && board[x][y] == p; x += dx, y += dy) c++;
                for (int x = i - dx, y = j - dy; check(x, y) && board[x][y] == p; x -= dx, y -= dy) c++;

                if (c >= 4) return QPoint(i, j);
            }
    }
    return QPoint(-1, -1);
}

int AIAlphaBeta::evaluate()
{
    int ret = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            int t = board[i][j];
            if (t == -1) continue;
            ret += potential(i, j, t) * (board[i][j] == 0 ? 1 : -1);
        }
    return ret;
}

bool AIAlphaBeta::gameOver()
{
    for (int i = 0; i < n; i++)
    {
        if (linkCheck(i, 0, 0, 1)) return true;
        if (linkCheck(0, i, 1, 0)) return true;
        if (linkCheck(i, 0, 1, 1)) return true;
        if (linkCheck(i, 0, -1, 1)) return true;
        if (linkCheck(0, i, 1, 1)) return true;
        if (linkCheck(n - 1, i, -1, 1)) return true;
    }
    return false;
}

bool AIAlphaBeta::linkCheck(int x, int y, int dx, int dy)
{
    int c = -1, s = 0;
    for (; check(x, y); x += dx, y += dy)
    {
        int t = board[x][y];
        if (t == c)
        {
            s++;
            if (s >= 5 && t != -1)
            {
                return true;
            }
        }
        else
        {
            c = t;
            s = 1;
        }
    }
    return false;
}


#undef n
#undef INF
#undef timelimit
#undef check
