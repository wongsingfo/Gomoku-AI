#ifndef CONSTANTS
#define CONSTANTS

const int BOARD_SIZE = 19;
const int TILE_SIZE = 30;
const int TILE_MARGIN = 3;

enum ChessType
{
    EMPTY,
    BLACK,
    WHITE,
};

enum PlayerType
{
    HUMAN,
    AI_sample,
    AI_alphabeta,
};

#endif // CONSTANTS

