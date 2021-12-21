/**
 * @file bits.c
 * @brief Implementation of squares and bitboards interface.
 */

#include "bits.h"

#include "../lib/contracts.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/** @brief The length of a rank */
static const uint8_t rank_length = 8;

/** @brief The length of a file */
static const uint8_t file_length = 8;

/** @brief The length a bitboard word */
static const uint8_t bitboard_length = 64;

/** @brief Strings for quick conversion from squares */
static char *squares_to_strings[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

/** 
 * @brief Masks used in divide-and-conquer algorithms regarding 64-bit words, 
 * e.g. bit-reversal, bit-counting
 */
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
    return s < bitboard_length;
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
    dbg_requires(is_square(s));
    bitboard b = 0;
    if (s >= 0) {
        b = (bitboard)(1UL << s);
    }
    return b;
}

char *square_to_string(square s) {
    dbg_requires(is_square(s));
    return squares_to_strings[s];
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
    square s = square_calculate(rank_from_char(str[1]), file_from_char(str[0]));
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

bitboard bitboard_set(bitboard b, square s) {
    dbg_requires(is_square(s));
    return b | square_to_bitboard(s);
}

bitboard bitboard_reset(bitboard b, square s) {
    dbg_requires(is_square(s));
    return b & ~square_to_bitboard(s);
}

square bitboard_to_square(bitboard b) {
    dbg_requires(bitboard_is_single(b));
    return bitboard_count_bits(b - 1);
}

uint8_t bitboard_count_bits(bitboard b) {
    b = (b & DQ1) + ((b & (DQ1 << 1)) >> 1);
    b = (b & DQ2) + ((b & (DQ2 << 2)) >> 2);
    b = (b & DQ3) + ((b & (DQ3 << 4)) >> 4);
    b = (b & DQ4) + ((b & (DQ4 << 8)) >> 8);
    b = (b & DQ5) + ((b & (DQ5 << 16)) >> 16);
    b = (b & DQ6) + ((b & (DQ6 << 32)) >> 32);
    dbg_ensures(b <= bitboard_length);
    return b;
}

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
    for (int r = 0; r < rank_length; r++) {
        for (int f = 0; f < file_length; f++) {
            board[r][f] = b % 2 ? 'x' : '.';
            b >>= 1;
        }
    }
    for (int r = rank_length - 1; r >= 0; r--) {
        for (int f = 0; f < file_length; f++) {
            printf("%c ", board[r][f]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}