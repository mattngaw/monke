/**
 * @file bits.c
 * @brief Implementation of squares and bitboards interface.
 */

#include "bits.h"

#include "../lib/contracts.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

const square INVALID_SQUARE = 64;

const bitboard BITBOARD_EMPTY = 0;
const bitboard BITBOARD_FULL = 0xFFFFFFFFFFFFFFFF;

const char *SQUARES_TO_STRINGS[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

static const uint8_t RANK_LENGTH = 8;

static const uint8_t FILE_LENGTH = 8;

static const uint8_t BITBOARD_SIZE = 64;

/* Masks used in divide-and-conquer algorithms regarding 64-bit words */
static const uint64_t DQ1 = 0x5555555555555555;
static const uint64_t DQ2 = 0x3333333333333333;
static const uint64_t DQ3 = 0x0F0F0F0F0F0F0F0F;
static const uint64_t DQ4 = 0x00FF00FF00FF00FF;
static const uint64_t DQ5 = 0x0000FFFF0000FFFF;
static const uint64_t DQ6 = 0x00000000FFFFFFFF;

/*
 * ---------------------------------------------------------------------------
 *                                   SQUARES
 * ---------------------------------------------------------------------------
 */

static bool is_rank(rank r) {
    return r < 8;
}

static bool is_file(file f) {
    return f < 8;
}

static bool is_square(square s) {
    return s < BITBOARD_SIZE;
}

square square_calculate(rank r, file f) {
    dbg_requires(is_rank(r));
    dbg_requires(is_file(f));
    square s = r * 8 + f;
    dbg_ensures(is_square(s));
    return s;
}

rank square_get_rank(square s) {
    dbg_requires(is_square(s));
    rank r = s / 8;
    dbg_ensures(is_rank(r));
    return r;
}

file square_get_file(square s) {
    dbg_requires(is_square(s));
    file f = s % 8;
    dbg_ensures(is_file(f));
    return f;
}

bitboard square_to_bitboard(square s) {
    if (s == INVALID_SQUARE) return BITBOARD_EMPTY;
    bitboard b = 1UL << s;
    return b;
}

const char *square_to_string(square s) {
    dbg_requires(is_square(s));
    return SQUARES_TO_STRINGS[s];
}

static rank rank_from_char(char c) {
    dbg_requires('1' <= c && c <= '8');
    rank r = c - '1';
    dbg_ensures(is_rank(r));
    return r;
}

static file file_from_char(char c) {
    dbg_requires('a' <= c && c <= 'h');
    file f = c - 'a';
    dbg_ensures(is_rank(f));
    return f;
}

square square_from_string(char *str) {
    dbg_requires(strlen(str) == 2);
    dbg_requires('a' <= str[0] && str[0] <= 'h');
    dbg_requires('1' <= str[1] && str[1] <= '8');
    rank r = rank_from_char(str[1]);
    file f = file_from_char(str[0]);
    square s = square_calculate(r, f);
    dbg_ensures(is_square(s));
    return s;
}

/*
 * ---------------------------------------------------------------------------
 *                                  BITBOARDS
 * ---------------------------------------------------------------------------
 */

static bool bitboard_is_single(bitboard b) {
    return (b != 0) && ((b & (b - 1)) == 0);
}

bool bitboard_is_empty(bitboard b) {
    return b == 0;
}

bitboard bitboard_set(bitboard b, square s) {
    if (s >= BITBOARD_SIZE) {
        return b;
    }
    return b | (1UL << s);
}

bitboard bitboard_reset(bitboard b, square s) {
    if (s >= BITBOARD_SIZE) {
        return b;
    }
    return b & ~(1UL << s);
}

square bitboard_to_square(bitboard b) {
    dbg_requires(__builtin_popcountll(b) <= 1);
    if (b == 0) return INVALID_SQUARE;
    square s = __builtin_ctzll(b);
    dbg_ensures(is_square(s));
    return s;
}

uint8_t bitboard_count_bits(bitboard b) {
    int count = __builtin_popcountll(b);
    dbg_ensures(count <= BITBOARD_SIZE);
    return (uint8_t) count;
}

square bitboard_bsf(bitboard b) {
    if (b == 0) return INVALID_SQUARE;
    return __builtin_ctzll(b);
}

square bitboard_bsr(bitboard b) {
    if (b == 0) return INVALID_SQUARE;
    return 63 - __builtin_clzll(b);
}

square bitboard_iter_first(bitboard *b) {
    square s = bitboard_bsf(*b);
    *b = bitboard_reset(*b, s);
    return s;
}

square bitboard_iter_last(bitboard *b) {
    square s = bitboard_bsr(*b);
    *b = bitboard_reset(*b, s);
    return s;
}

/* Divide-and-conquer algorithm using bitmasks */
bitboard bitboard_rotate(bitboard b) {
    b = ((b & DQ1) << 1) | ((b >> 1) & DQ1);
    b = ((b & DQ2) << 2) | ((b >> 2) & DQ2);
    b = ((b & DQ3) << 4) | ((b >> 4) & DQ3);
    b = ((b & DQ4) << 8) | ((b >> 8) & DQ4);
    b = ((b & DQ5) << 16) | ((b >> 16) & DQ5);
    b = ((b & DQ6) << 32) | ((b >> 32) & DQ6);
    return b;
}

void bitboard_print(bitboard b) {
    char board[8][8];
    
    /* Build array of chars */
    for (int r = 0; r < RANK_LENGTH; r++) {
        for (int f = 0; f < FILE_LENGTH; f++) {
            square s = square_calculate(r,f);
            bitboard s_bb = square_to_bitboard(s);
            board[r][f] = b & s_bb ? 'x' : '.';
        }
    }

    /* Print chars with appropriate spacing */
    for (int r = RANK_LENGTH - 1; r >= 0; r--) {
        for (int f = 0; f < FILE_LENGTH; f++) {
            printf("%c ", board[r][f]);
        }
        printf("\n");
    }
    printf("\n");
    
    return;
}