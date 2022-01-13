/**
 * @file position.h
 * @brief Provides an interface for positions.
 * 
 * Positions are implemented to be color-agnostic, meaning that at any time, 
 * the computer evaluates the position as a collection of "our" pieces and
 * "their" pieces. When a move is made, the computer swaps its perspective
 * and evaluates from the other POV.
 * 
 * The game itself, however, is not color-agnostic. The computer will only know
 * and show who's white and who's black when outputting the position to the
 * user.
 */

#ifndef _POSITION_H_
#define _POSITION_H_

#include "bits.h"

#include <stdbool.h>
#include <stdio.h>

/** @brief Represents side-to-move (absolute) */
typedef enum Color {
    WHITE,
    BLACK
} Color;
extern const uint8_t NUM_COLORS;

/** @brief Represents the "side to calculate/optimize for" and the "opponent" */
typedef enum Whose {
    OURS,
    THEIRS
} Whose;
extern const uint8_t NUM_WHOSE;

/** @brief Labelling the different pieces */
typedef enum Piece {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} Piece;
extern const uint8_t NUM_PIECES;

/** @brief Labelling the different castling sides */
typedef enum Castling {
    KINGSIDE,
    QUEENSIDE
} Castling;
extern const uint8_t NUM_CASTLINGS;

/** @brief A structure for everything to do with a board position */
typedef struct position {
    bitboard whose[2];
    bitboard pieces[5];
    square   king[2];
    uint16_t halfmoves;
    uint16_t fullmoves;
    uint8_t  castling;    // a four-bit word
    Color    color;       // WHITE or BLACK

} position;

/**
 * @brief The ASCII characters of pieces of different colors
 * 
 * Used when printing positions and moves
 */
extern const char PIECE_CHARS[2][6];

/*
 * ---------------------------------------------------------------------------
 *                                 POSITION
 * ---------------------------------------------------------------------------
 */

/** @brief Dynamically allocates a new position */
position *position_new(void);

/** @brief Frees a position */
void position_free(position *P);

/** @brief Empties a position (no pieces, no flags set) */
void position_clear(position *P);

/** @brief Initializes a position to the starting position of a game */
void position_init(position *P);

/** @brief Initiliazes a position according to the given FEN string */
void position_from_fen(position *P, const char *fen);

/** [TODO] @brief Exports a string for a given position. */
char *position_to_fen(position *P);

/**
 * @brief Retrieves the pieces in a position filtered by piece and possesion
 * 
 * @param[in] P
 * @param[in] whose
 * @param[in] piece
 * @pre P != NULL
 * 
 * @return b (Bitboard representing all of `whose pieces of type `piece`)
 */
bitboard position_get_pieces(position *P, Whose whose, Piece piece);

/** @brief XORs the requested pieces with the given bitboard */
void position_set_pieces(position *P, Whose whose, Piece piece, bitboard b);

/**
 * @brief Gets the capture square of en passant if there is one
 * 
 * @param[in] P
 * @param[in] whose
 * @pre P != NULL
 * 
 * @return The en passant capture square or INVALID_SQUARE if there is none
 */
square position_get_en_passant(position *P, Whose whose);

/**
 * @brief Sets the en passant flag given the square of a double pawn push
 * 
 * @param[in] P
 * @param[in] whose
 * @param[in] dpp_to
 * @pre P != NULL
 * @pre (whose == OURS && 40 <= dpp_to < 48) || 
 *      (whose == THEIRS && 32 <= dpp_to < 40)
 * 
 */
void position_set_en_passant(position *P, Whose whose, square dpp_to);

/** @brief Resets all en_passant flags */
void position_reset_en_passant(position *P);

/** @brief Gets the singular bitboard with the king's square set to true */
bitboard position_get_king(position *P, Whose whose);

/**
 * @brief Sets the king's position to a square
 * 
 * @param[in] P
 * @param[in] whose
 * @param[in] s
 * @pre 0 <= s < 64
 * @pre P != NULL
 */
void position_set_king(position *P, Whose whose, square s);

/**
 * @brief Gets the castling status of a given possesion for a certain side
 * 
 * @param[in] P
 * @param[in] whose
 * @param[in] castling
 * @pre P != NULL
 * 
 * @return true if can castle else false
 */
bool position_get_castling(position *P, Whose whose, Castling castling);

/** @brief Sets the castling status of a given possesion for a certain side */
void position_set_castling(position *P, Whose whose, Castling castling, bool can_castle);

/** @brief Rotate the position (rotates bitboards and swaps castling flags). */
void position_rotate(position *P);

/** @brief Prints the position as a human-readable chess position. */
void position_print(position *P);

#endif