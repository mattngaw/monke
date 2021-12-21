/**
 * @file position.h
 * @brief Provides an interface for positions.
 */

#ifndef _POSITION_H_
#define _POSITION_H_

#include "bits.h"

#include <stdbool.h>

/*
 * ---------------------------------------------------------------------------
 *                                    BOARDS
 * ---------------------------------------------------------------------------
 */

/** @brief A structure for everything to do with a board position */
typedef struct {
    bitboard ours,
             theirs,
             pawns,
             knights,
             bishops,
             rooks,
             queens;
    uint16_t halfmoves;
    uint16_t fullmoves;
    square   our_king;
    square   their_king;
    square   en_passant;
    uint8_t   castling;      // a four-bit word
    bool     rotated;       // True means black to move.

} position;

typedef position *position_t;

static char starting_fen[] = 
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief Allocates a new position on the heap.
 * 
 * @return position_t 
 */
position_t position_new(void);

/**
 * @brief Frees a position.
 * 
 * @param[in] P
 */
void position_free(position_t P);

/**
 * @brief Empties a position (sets all the bits to zero).
 * 
 * @param[in] P
 */
void position_clear(position_t P);

/**
 * @brief Initializes a position to the starting position of a game.
 * 
 * @param[in] P
 */
void position_init(position_t P);

/**
 * @brief Initiliazes a position according to the given FEN string.
 * 
 * @param[in] P
 * @param[in] fen
 */
void position_from_fen(position_t P, char *fen);

/**
 * @brief Exports a string for a given position.
 * 
 * @param[in] P
 * @return A FEN string.
 */
char *position_to_fen(position_t P);

/** @brief Retrieves our pawns. */
bitboard position_get_our_pawns(position_t P);

/** @brief Sets our pawns. */
void position_set_our_pawns(position_t P, bitboard b);

/** @brief Retrieves their pawns. */
bitboard position_get_their_pawns(position_t P);

/** @brief Sets their pawns. */
void position_set_their_pawns(position_t P, bitboard b);

/** @brief Retrieves our en passant flag. */
square position_get_our_en_passant(position_t P);

/** @brief Sets our en passant flag. */
void position_set_our_en_passant(position_t P, square s);

/** @brief Retrieves their en passant flag. */
square position_get_their_en_passant(position_t P);

/** @brief Sets their en passant flag. */
void position_set_their_en_passant(position_t P, square s);

/** @brief Retrieves our knights. */
bitboard position_get_our_knights(position_t P);

/** @brief Sets our knights. */
void position_set_our_knights(position_t P, bitboard b);

/** @brief Retrieves their knights. */
bitboard position_get_their_knights(position_t P);

/** @brief Sets their knights. */
void position_set_their_knights(position_t P, bitboard b);
 
/** @brief Retrieves our bishops. */
bitboard position_get_our_bishops(position_t P);

/** @brief Sets our bishops. */
void position_set_our_bishops(position_t P, bitboard b);

/** @brief Retrieves their bishops. */
bitboard position_get_their_bishops(position_t P);

/** @brief Sets our bishops. */
void position_set_their_bishops(position_t P, bitboard b);

/** @brief Retrieves our rooks. */
bitboard position_get_our_rooks(position_t P);

/** @brief Set our rooks. */
void position_set_our_rooks(position_t P, bitboard b);

/** @brief Retrieves their rooks. */
bitboard position_get_their_rooks(position_t P);

/** @brief Set their rooks. */
void position_set_their_rooks(position_t P, bitboard b);

/** @brief Retrieves our queens. */
bitboard position_get_our_queens(position_t P);

/** @brief Set our queens. */
void position_set_our_queens(position_t P, bitboard b);

/** @brief Retrieves their queens. */
bitboard position_get_their_queens(position_t P);

/** @brief Set their queens. */
void position_set_their_queens(position_t P, bitboard b);

/** @brief Retrieves our king. */
bitboard position_get_our_king(position_t P);

/** @brief Set our king. */
void position_set_our_king(position_t P, square s);

/** @brief Retrieves their king. */
bitboard position_get_their_king(position_t P);

/** @brief Set their king. */
void position_set_their_king(position_t P, square s);

/** @brief Whether or not we can castle kingside. */
bool position_get_our_OO(position_t P);

/** @brief Set whether or not we can castle kingside. */
void position_set_our_OO(position_t P, bool can_OO);

/** @brief Whether or not we can castle queenside. */
bool position_get_our_OOO(position_t P);

/** @brief Set whether or not we can castle queenside. */
void position_set_our_OOO(position_t P, bool can_OOO);

/** @brief Whether or not they can castle kingside. */
bool position_get_their_OO(position_t P);

/** @brief Set whether or not they can castle kingside. */
void position_set_their_OO(position_t P, bool can_OO);

/** @brief Whether or not they can castle queenside. */
bool position_get_their_OOO(position_t P);

/** @brief Set whether or not they can castle queenside. */
void position_set_their_OOO(position_t P, bool can_OOO);

/** @brief Rotate the position (rotates bitboards and swaps castling flags). */
void position_rotate(position_t P);

/** @brief Prints the position as a human-readable chess position. */
void position_print(position_t P);

#endif