/**
 * @file bits.h
 * @brief Provides an interface for bitboards and squares.
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
 * @brief A square on a chessboard, where a1, a2, ..., h7, h8 are mapped by the 
 * range from 0 to 63
 */
typedef uint8_t square;

/** 
 * @brief A 64-bit word representing a set of squares
 * 
 * Used to represent piece locations, legal moves, attack maps, etc.
 */
typedef uint64_t bitboard;

/** 
 * @brief Global enumeration for easy square access
 */
typedef enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
} Square;

extern const square INVALID_SQUARE;

/** @brief Helpful bitboard constants */
extern const bitboard BITBOARD_EMPTY;
extern const bitboard BITBOARD_FULL;

extern const char *SQUARES_TO_STRINGS[64];

/*
 * ---------------------------------------------------------------------------
 *                                   SQUARES
 * ---------------------------------------------------------------------------
 */

/**
 * @brief Calculates a square given the rank and file.
 * 
 * @param[in] r
 * @param[in] f
 * @return square 
 */
square square_calculate(rank r, file f);

/**
 * @brief Calculates a rank given a square.
 * 
 * @param[in] s 
 * @return rank 
 */
rank square_get_rank(square s);

/**
 * @brief Calculates a file given a square.
 * 
 * @param[in] s
 * @return file 
 */
file square_get_file(square s);

/**
 * @brief Converts a square to a bitboard.
 * 
 * @param[in] s
 * @return bitboard 
 */
bitboard square_to_bitboard(square s);

/**
 * @brief Converts a square to its string equivalent.
 * 
 * @param[in] s
 * @return char* 
 */
const char *square_to_string(square s);

/**
 * @brief Converts a string to a square.
 * 
 * @param[in] str
 * @return square 
 */
square square_from_string(char *str);

/*
 * ---------------------------------------------------------------------------
 *                                  BITBOARDS
 * ---------------------------------------------------------------------------
 */

bool bitboard_is_empty(bitboard b);

/**
 * @brief Sets the s'th bit of b to true.
 * 
 * @param[in] b
 * @param[in] s
 * @return square 
 */
bitboard bitboard_set(bitboard b, square s);

/**
 * @brief Sets the s'th bit of b to false.
 * 
 * @param[in] b
 * @param[in] s
 * @return square 
 */
bitboard bitboard_reset(bitboard b, square s);

/**
 * @brief Converts a single bitboard to a square.
 * 
 * @param[in] b
 * @return square 
 */
square bitboard_to_square(bitboard b);

/**
 * @brief Counts the number of bits in a bitboard.
 * 
 * @param[in] b
 * @return uint8_t 
 */
uint8_t bitboard_count_bits(bitboard b);

/**
 * @brief Gets the index of the least significant bit.
 * 
 * @param[in] b
 * @return square
 */
square bitboard_bsf(bitboard b);

/**
 * @brief Gets the index of the most significant bit.
 * 
 * @param[in] b
 * @return square
 */
square bitboard_bsr(bitboard b);

square bitboard_iter_first(bitboard *b);

square bitboard_iter_last(bitboard *b);

/**
 * @brief Reverses the bits of a bitboard.
 * 
 * @param[in] b
 * @return bitboard 
 */
bitboard bitboard_rotate(bitboard b);

/**
 * @brief Prints a bitboard.
 * 
 * @param[in] b
 */
void bitboard_print(bitboard b);

#endif