#include "aigo.h"
#include <QVector>
#include <QPoint>
#include <QDebug>
#include <stack>
#include "constants.h"


AIGO::AIGO(QObject *parent) : AIThread(parent)
{

}


static constexpr int EMPTYMARKER = -1;
static constexpr int INF = 10000;

static char board[BOARD_SIZE][BOARD_SIZE];

static inline bool check(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE) return false;
    if (y < 0 || y >= BOARD_SIZE) return false;
    return true;
}

static inline bool isEmpty(int x, int y) {
    return board[x][y] == EMPTYMARKER;
}

static bool nearByChess(int x, int y)
{
    for (int dx = -2; dx <= 2; dx++)
        for (int dy = -2; dy <= 2; dy++)
        {
            int p = x + dx, q = y + dy;
            if (check(p, q) && board[p][q] != EMPTYMARKER) return true;
        }
    return false;
}


struct Threat {
    int x, y;
    int level;

    Threat(int x, int y, int level) : x(x), y(y), level(level) {}

    bool operator < (const Threat &o) const {
        return level < o.level;
    }
};

typedef Threat Choice;

static int potential(const int &x, const int &y, const int &p) {
    int ret = 0;
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
        {
            if (! dx && ! dy) continue;
            int tmp = 0;
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
                    tmp += (6 - s) * (6 - s);
                }
            }
            ret += tmp;
        }
    return ret;
}

static const int d4[][2] = {
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
};

static int potential2(const int &x, const int &y, const int &p) {
    int ret = 0;
    int dx, dy;
    for (int d = 0; d < 4; d++)
    {
        int c = 0, cs = 1;
        dx = d4[d][0]; dy = d4[d][1];
        for (int s = 1, px = x + dx, py = y + dy; s < 5; s++, px += dx, py += dy)
        {
            if (! check(px, py)) break;
            if (board[px][py] == (p ^ 1)) break;
            cs++;
            if (board[px][py] == p) c++;
        }
        dx = -dx; dy = -dy;
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

static int evaluateGame(int p) {
    int ret = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            int t = board[i][j];
            if (t == EMPTYMARKER) continue;
            if (t == p) {
                ret += potential(i, j, t);
            } else {
                ret -= potential(i, j, t);
            }
        }
    return ret;
}


static std::stack<Threat> history;

static void playStep(Threat t, int p) {
    Q_ASSERT(board[t.x][t.y] == EMPTYMARKER);
    board[t.x][t.y] = static_cast<char> (p);
    history.push(t);
}

static void playStepBack() {
    Threat t = history.top();
    history.pop();
    board[t.x][t.y] = EMPTYMARKER;
}

static void threatSearch (int p, std::vector<Threat> &r,
                          int sx, int sy, int dx, int dy) {
    int trail = 0xaaa;
    int x = sx, y = sy;

    /* Encoding:
     *   _ => 00
     *   o => 01
     *   x => 10
     *
     * xxxxxx => 0xaaa
     *
     * 5 bits mask => 0x3ff
     * 6 bits mask => 0xfff
     *
     * Level 5:
     * ooooo  => 01 01 01 01 01 => 0x155
     *
     * Level 4:
     * oooo_  => 01 01 01 01 00 => 0x154, 0x55
     * ooo_o  => 01 01 01 00 01 => 0x151, 0x115
     * oo_oo  =>                   0x145
     *
     * Level 3:
     * _ooo_  => 00 01 01 01 00 => 0x54
     * _oo_o_ => 00 01 01 00 01 00 => 0x144, 0x114
     */



    while (check(x, y)) {
        trail <<= 2;
        if (board[x][y] == p) {
            trail |= 1;
        } else if (board[x][y] == EMPTYMARKER) {
            trail |= 0;
        } else {  //board[x][y] == p ^ 1
            trail |= 2;
        }


        switch (trail&0x3ff) {
        case 0x155: r.push_back(Threat(0, 0, 5)); return;
        case 0x154: r.push_back(Threat(x, y, 4)); break;
        case 0x55:  r.push_back(Threat(x-dx*4, y-dy*4, 4)); break;
        case 0x151: r.push_back(Threat(x-dx, y-dy, 4)); break;
        case 0x115: r.push_back(Threat(x-dx*3, y-dy*3, 4)); break;
        case 0x145: r.push_back(Threat(x-dx*2, y-dy*2, 4)); break;
        case 0x54:
            r.push_back(Threat(x, y, 3));
            r.push_back(Threat(x-dx*4, y-dy*4, 3));
            break;
        }

        if ((trail&0xfff) == 0x144) {
            r.push_back(Threat(x, y, 3));
            r.push_back(Threat(x-dx*2, y-dy*2, 3));
            r.push_back(Threat(x-dx*5, y-dy*5, 3));
        } else if ((trail&0xfff) == 0x114) {
            r.push_back(Threat(x, y, 3));
            r.push_back(Threat(x-dx*3, y-dy*3, 3));
            r.push_back(Threat(x-dx*5, y-dy*5, 3));
        }

        x += dx;
        y += dy;
    }
}

static std::vector<Threat> threatSearch(int p) {

    std::vector<Threat> r;
//    return r;

    for (int i = 0; i < BOARD_SIZE; i++) {
        threatSearch(p, r, i, 0, 0, 1);
        threatSearch(p, r, i, 0, 1, 1);
    }
    for (int j = 0; j < BOARD_SIZE; j++) {
        threatSearch(p, r, 0, j, 1, 0);
        threatSearch(p, r, 0, j, 1, 1);
    }

    return r;
}

static int search(
        int p,
        int alpha, int beta,
        int remaining);

static void search(int p, int* alpha_addr, int beta, int remaining,
                   std::vector<Threat> &trial, int delta) {
    int alpha = *alpha_addr;
    for (Threat t : trial) {
        playStep(t, p);
        int score = - search(p^1, -beta, -alpha, remaining - delta);
        playStepBack();

        if (score >= beta) {
            *alpha_addr = beta;
            return;
        }
        alpha = std::max(alpha, score);


    }
    *alpha_addr = alpha;
}

static int search(
        int p, // we take which side
        int alpha, int beta,
        int remaining) // remaining steps we want to try
{
    if (remaining < 0) {
//        for (int i = 0; i < BOARD_SIZE; i++) {
//            for (int j = 0; j < BOARD_SIZE; j++) {
//                int t = board[i][j];
//                printf("%c", t == p ? 'o' : (t == (p^1) ? 'x' : '_'));
//            }
//            printf("\n");
//        }
//        exit(0);
        int r = evaluateGame(p);
//        qDebug() << r;
        return r;
    }

    auto myThreats = threatSearch(p);
    auto rivalThreats = threatSearch(p^1);

    std::sort(myThreats.begin(), myThreats.end());
    std::sort(rivalThreats.begin(), rivalThreats.end());

    int myLevel = myThreats.empty() ? 0 : myThreats.back().level;
    int rivalLevel = rivalThreats.empty() ? 0 : rivalThreats.back().level;
//    qDebug() << myLevel << rivalLevel;

    if (myLevel == 5) {
        return INF;
    }
    if (rivalLevel == 5) {
        qDebug() << "INF" ;
        return -INF;
    }

    if (myLevel >= rivalLevel) {
        search(p, &alpha, beta, remaining, myThreats, 1);
        if (alpha == beta) return alpha;

        std::vector<Choice> que;
        for (int i = 0; i < BOARD_SIZE; i++)
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (! isEmpty(i, j)) continue;
                if (! nearByChess(i, j)) continue;
                int t = std::max(potential(i, j, p), potential(i, j, p ^ 1));
                t += std::max(potential2(i, j, p), potential2(i, j, p ^ 1)) * 2;
                que.push_back(Choice(i, j, t));
            }
        std::sort(que.rbegin(), que.rend());
        que.resize(std::min(que.size(), static_cast<size_t>(10)), Threat(0,0,0));

        search(p, &alpha, beta, remaining, que, 3);
    } else {
        search(p, &alpha, beta, remaining, rivalThreats, 1);
    }
    return alpha;
}

static int evaluate() {
    return search(1, -INF, INF, 13);
}

void AIGO::run()
{
    qDebug() << "AIGO begin";
    bool is_empty = true;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (chessboard[i][j] == BLACK) {
                is_empty = false;
                board[i][j] = 0;
            }
            else if (chessboard[i][j] == WHITE) {
                is_empty = false;
                board[i][j] = 1;
            }
            else board[i][j] = EMPTYMARKER;
        }
    // we take black

    if (is_empty) response(BOARD_SIZE / 2, BOARD_SIZE / 2);

    int best_i, best_j, worst_score = INF + 1;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (nearByChess(i, j) && isEmpty(i, j)) {
                board[i][j] = 0;
                int score = evaluate();
                if (score < worst_score) {
                    worst_score = score;
                    best_i = i;
                    best_j = j;
                }

//                qDebug() << i << j << worst_score;
                board[i][j] = EMPTYMARKER;
            }
        }
    qDebug() << "best score: " << -worst_score;
    response(best_i, best_j);
    qDebug() << "AIGO end";
}
