/**
 * @file moves.h
 * @brief Provides an interface for generating and applying legal moves.
 */

#ifndef _MOVES_H_
#define _MOVES_H_

#include "position.h"

#include <stdbool.h>
 
typedef enum GameState {
    CONTINUE,
    DRAW,
    CHECKMATE
} GameState;

/** 
 * @brief Representation of a move 
 * 
 * Moves also come with flags that represent special properties of the move:
 * Value | Promotion Bit | Capture Bit | Misc Bit 1 | Misc Bit 0 | Description
 *   0   |      0        |      0      |     0      |     0      | quiet move
 *   1   |      0        |      0      |     0      |     1      |    dpp
 *   2   |      0        |      0      |     1      |     0      |    O-O
 *   3   |      0        |      0      |     1      |     1      |   O-O-O
 *   4   |      0        |      1      |     0      |     0      |  capture
 *   5   |      0        |      1      |     0      |     1      | en-passant
 *   skip 6 and 7
 *   8   |      1        |      0      |     0      |     0      | N promo
 *   9   |      1        |      0      |     0      |     1      | B promo
 *   10  |      1        |      0      |     1      |     0      | R promo
 *   11  |      1        |      0      |     1      |     1      | Q promo
 *   12  |      1        |      1      |     0      |     0      | N x promo
 *   13  |      1        |      1      |     0      |     1      | B x promo
 *   14  |      1        |      1      |     1      |     0      | R x promo
 *   15  |      1        |      1      |     1      |     1      | Q x promo
 */
typedef struct move {
    Piece piece;
    square from;
    square to;
    uint8_t flags;
} move;

/**
 * @brief A dynamic array housing the legal or pseudo-legal moves
 * 
 * Array size doubles or halves when resizing
 */
typedef struct movelist {
    move *array;
    int size;
    int limit;
} movelist;
typedef movelist *movelist_t;

/** @brief An invalid move returned by popping from an empty movelist */
extern const move NULL_MOVE;

/** @brief Move flags */
extern const uint8_t M_FLAG_QUIET;          // 0
extern const uint8_t M_FLAG_DPP;            // 1
extern const uint8_t M_FLAG_CASTLING[2];    // 2, 3
extern const uint8_t M_FLAG_CAPTURE;        // 4
extern const uint8_t M_FLAG_EN_PASSANT;     // 5
extern const uint8_t M_FLAG_PROMOTION[5];   // 8, 9, 10, 11
                                            // Promo-captures (+ M_FLAG_CAPTURE)

/*
 * ---------------------------------------------------------------------------
 *                                   MOVES
 * ---------------------------------------------------------------------------
 */

/** @brief Applies a pseudo-legal move and returns the old position */
position move_make(position *P, move m);

/** @brief Prints a move in human-readable format */
void move_print(move m, Color c);

/*
 * ---------------------------------------------------------------------------
 *                                 MOVELIST
 * ---------------------------------------------------------------------------
 */

/** @brief Dynamically allocates a movelist */
movelist_t movelist_new(void);

/** @brief Gets the length of the array */
int movelist_length(movelist_t M);

/** @brief Empties the movelist */
void movelist_clear(movelist_t M);

/** @brief Frees a movelist */
void movelist_free(movelist_t M);


/** @brief Prints all the moves in the movelist */
void movelist_print(movelist_t M, Color c);

/*
 * ---------------------------------------------------------------------------
 *                                 MOVE GEN
 * ---------------------------------------------------------------------------
 */

/** @brief Returns all the squares attacked by the pieces of `whose` */
bitboard build_attack_map(position *P, Whose whose);

/** @brief Whether a king is in check or not */
bool king_in_check(position *P, Whose whose);

GameState get_game_state(position *P, movelist_t M);

/** @brief Populates a movelist with all legal moves for OUR pieces */
movelist_t generate_moves(movelist_t M, position *P);

#endif
