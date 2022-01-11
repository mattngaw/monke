/**
 * @file zobrist.c
 * @brief Provides the implementation for hashing positions.
 */

#include "bits.h"
#include "position.h"
#include "zobrist.h"

#include "../lib/contracts.h"

#include <stdlib.h>
#include <time.h>

static uint64_t SEED;

/** @brief PRNs for all piece types of both color on any square */
static uint64_t PIECE_PRN[2][6][64]; // contains en-passant in pawns bitboard

/** @brief PRNs for castling rights for both colors */
static uint64_t CASTLING_PRN[2][2];

/** @brief PRN for black */
static uint64_t COLOR_PRN;

/**
 * @brief XORShift implementation: https://en.wikipedia.org/wiki/Xorshift
 */
void hash_init(void) {
    uint64_t x;
    SEED = (uint64_t) time(NULL);

    for (int c = 0; c < NUM_COLORS; c++) {
        for (int p = 0; p < NUM_PIECES; p++)  {
            for (int s = 0; s < BITBOARD_SIZE; s++) {
                x = SEED;
                x ^= x << 13;
                x ^= x >> 7;
                x ^= x << 17;
                PIECE_PRN[c][p][s] = SEED = x;
            }
        }
        for (int o = 0; o < NUM_CASTLINGS; o++) {
            x = SEED;
            x ^= x << 13;
            x ^= x >> 7;
            x ^= x << 17;
            CASTLING_PRN[c][o] = SEED = x;
        }
    }
    
    x = SEED;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    COLOR_PRN = SEED = x;

    return;
}

/**
 * @brief Zobrist hashing: https://www.chessprogramming.org/Zobrist_Hashing
 */
zhash hash_position(position *P) {
    square s;
    bitboard bb;
    zhash Z = 0;
    position _P = *P;
    if (_P.color == BLACK) {
        Z ^= COLOR_PRN;
        position_rotate(&_P);
    }

    for (int c = 0; c < NUM_COLORS; c++) {
        for (int p = 0; p < KING; p++)  {   // For each of the pieces minus king
            bb = _P.whose[c] & _P.pieces[p];
            while ((s = bitboard_iter_first(&bb)) != INVALID_SQUARE) {
                Z ^= PIECE_PRN[c][p][s];
            }
        }
        Z ^= PIECE_PRN[c][KING][_P.king[c]]; // KING is special case

        for (int o = 0; o < NUM_CASTLINGS; o++) {   // For each of the castlings
            if (position_get_castling(&_P, c, o)) Z ^= CASTLING_PRN[c][o];
        }
    }

    return Z;
}