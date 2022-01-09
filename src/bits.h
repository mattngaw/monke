/**
 * @file bits.h
 * @brief Interface for squares and bitboards.
 */

#ifndef _BITS_H_
#define _BITS_H_

#include <stdbool.h>
#include <stdint.h>

/** @brief The rank (row) of a square */
typedef uint8_t rank;

/** @brief The file (column) of a square */
typedef uint8_t file;

/** 
 * @brief A square on a chessboard
 * 
 * a1, a2, ..., h7, h8 are mapped by the range from 0 to 63
 */
typedef uint8_t square;

/** 
 * @brief A 64-bit word representing a set of squares
 * 
 * Used to represent piece locations, attack maps, moves, etc.
 * Bits increase in row/rank-major order
 */
typedef uint64_t bitboard;

/** @brief Global enumeration for easy square access for white */
typedef enum WhiteSquare {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
} WhiteSquare;

/** @brief Global enumeration for easy square access for black */
typedef enum BlackSquare {
    h8, g8, f8, e8, d8, c8, b8, a8,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h1, g1, f1, e1, d1, c1, b1, a1
} BlackSquare;

/** @brief An illegal square, used as an extraneous return value */
extern const square INVALID_SQUARE;

/** @brief Helpful bitboard constants */
extern const bitboard BITBOARD_EMPTY;
extern const bitboard BITBOARD_FULL;

/** 
 * @brief Maps squares to their string representation 
 * 
 * e.g. SQUARES_TO_STRINGS[A1] == "a1"
 */
extern const char *SQUARES_TO_STRINGS[64];

/*
 * ---------------------------------------------------------------------------
 *                                   SQUARES
 * ---------------------------------------------------------------------------
 */

/**
 * @brief Calculates a square given the rank and file
 * 
 * @param[in] r
 * @param[in] f
 * @pre 0 <= r, f < 8
 * 
 * @return s
 * @post 0 <= s < 64
 */
square square_calculate(rank r, file f);

/**
 * @brief Calculates a rank given a square
 * 
 * @param[in] s 
 * @pre 0 <= s < 64
 * 
 * @return r
 * @post 0 <= r < 8
 */
rank square_get_rank(square s);

/**
 * @brief Calculates a file given a square
 * 
 * @param[in] s 
 * @pre 0 <= s < 64
 * 
 * @return f
 * @post 0 <= f < 8
 */
file square_get_file(square s);

/**
 * @brief Converts a square to a bitboard
 * 
 * @param[in] s
 * 
 * @return b (if 0 <= s < 64, bitboard with only the sth bit set)
 * @return b (otherwise BITBOARD_EMPTY)
 */
bitboard square_to_bitboard(square s);

/**
 * @brief Converts a square to its string equivalent
 * 
 * e.g. square_to_string(E4) == "e4"
 * 
 * @param[in] s
 * @pre 0 <= s < 64
 * 
 * @return str (s in string form)
 */
const char *square_to_string(square s);

/**
 * @brief Converts a string to a square
 * 
 * @param[in] str
 * @pre len(str) == 2
 * @pre 'a' <= str[0] <= 'h'
 * @pre '1' <= str[1] <= '8'
 * 
 * @return s 
 * @post 0 <= s < 64 
 */
square square_from_string(char *str);

/*
 * ---------------------------------------------------------------------------
 *                                  BITBOARDS
 * ---------------------------------------------------------------------------
 */

/**
 * @brief Checks if a bitboard is empty
 * 
 * @param[in] b
 * 
 * @return true if b == 0 else false
 */
bool bitboard_is_empty(bitboard b);

/**
 * @brief Sets the s'th bit of b to true
 * 
 * @param[in] b
 * @param[in] s
 * 
 * @return b with s bit set to true
 * @return b if s it not a valid square
 */
bitboard bitboard_set(bitboard b, square s);

/**
 * @brief Sets the s'th bit of b to false
 * 
 * @param[in] b
 * @param[in] s
 * 
 * @return b with s bit set to false
 * @return b if s it not a valid square
 */
bitboard bitboard_reset(bitboard b, square s);

/**
 * @brief Converts a single bitboard to a square
 * 
 * @param[in] b
 * @pre popcount(b) <= 1
 * 
 * @return s
 * @post 0 <= s < 64, or INVALID_SQUARE (iff b == 0)
 */
square bitboard_to_square(bitboard b);

/**
 * @brief Counts the number of bits in a bitboard
 * 
 * Makes use of the GCC builtin popcount
 * 
 * @param[in] b
 * 
 * @return count (number of true bits)
 * @post 0 <= count <= 64
 */
uint8_t bitboard_count_bits(bitboard b);

/**
 * @brief Gets the index of the least significant bit
 * 
 * BitScan Forward
 * Makes use of the GCC builtin ctz (count trailing zeroes)
 * 
 * @param[in] b
 * 
 * @return s
 * @post 0 <= s < 64, or INVALID_SQUARE (if b == 0)
 */
square bitboard_bsf(bitboard b);

/**
 * @brief Gets the index of the most significant bit
 * 
 * BitScan Reverse
 * Makes use of the GCC builtin clz (count leading zeroes)
 * 
 * @param[in] b
 * 
 * @return s
 * @post 0 <= s < 64, or INVALID_SQUARE (if b == 0)
 */
square bitboard_bsr(bitboard b);

/**
 * @brief Pops and returns the place of the least-significant true bit
 * 
 * bsf and resets
 * 
 * @param[in] b
 * 
 * @return s 
 * @post 0 <= s < 64, or INVALID_SQUARE (if b == 0)
 */
square bitboard_iter_first(bitboard *b);

/**
 * @brief Pops and returns the place of the most-significant true bit
 * 
 * bsf and resets
 * 
 * @param[in] b
 * 
 * @return s
 * @post 0 <= s < 64, or INVALID_SQUARE (if b == 0)
 */
square bitboard_iter_last(bitboard *b);

/**
 * @brief Reverses the bits of a bitboard, effectively rotating the board
 * 
 * @param[in] b
 * 
 * @return b_
 * @post bitboard_rotate(b_) == b 
 */
bitboard bitboard_rotate(bitboard b);

/** @brief Prints a bitboard */
void bitboard_print(bitboard b);

#endif