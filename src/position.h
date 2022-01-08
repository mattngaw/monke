/**
 * @file position.h
 * @brief Provides an interface for positions.
 */

#ifndef _POSITION_H_
#define _POSITION_H_

#include "bits.h"

#include <stdbool.h>
#include <stdio.h>

/*
 * ---------------------------------------------------------------------------
 *                                    BOARDS
 * ---------------------------------------------------------------------------
 */

/** */

typedef enum Color {
    WHITE,
    BLACK
} Color;

typedef enum Whose {
    OURS,
    THEIRS
} Whose;

typedef enum Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} Piece;

typedef enum Castling {
    KINGSIDE,
    QUEENSIDE
} Castling;

/** @brief A structure for everything to do with a board position */
typedef struct position {
    bitboard whose[2];
    bitboard pieces[5];
    square   king[2];
    uint16_t halfmoves;
    uint16_t fullmoves;
    uint8_t  castling;      // a four-bit word
    bool     rotated;       // True means black to move.

} position;
typedef position *position_t;

extern const char PIECE_CHARS[2][6];

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
void position_from_fen(position_t P, const char *fen);

/**
 * @brief Exports a string for a given position.
 * 
 * @param[in] P
 * @return A FEN string.
 */
char *position_to_fen(position_t P);

bitboard position_get_pieces(position_t P, Whose whose, Piece piece);

void position_set_pieces(position_t P, Whose whose, Piece piece, bitboard b);

square position_get_en_passant(position_t P, Whose whose);

void position_set_en_passant(position_t P, Whose whose, square to);

void position_reset_en_passant(position_t P);

bitboard position_get_king(position_t P, Whose whose);

void position_set_king(position_t P, Whose whose, square s);

bool position_get_castling(position_t P, Whose whose, Castling castling);

void position_set_castling(position_t P, Whose whose, Castling castling, bool can_castle);

/** @brief Rotate the position (rotates bitboards and swaps castling flags). */
void position_rotate(position_t P);

/** @brief Prints the position as a human-readable chess position. */
void position_print(position_t P);

#endif