/**
 * @file moves.c
 * @brief Provides an interface for generating and applying moves.
 * 
 * 
 */

#ifndef _MOVES_H_
#define _MOVES_H_

#include "bits.h"
#include "position.h"

#include <stdbool.h>
 
typedef struct move {
    Piece piece;
    square from;
    square to;
    uint8_t flags;
} move;

typedef struct movelist {
    move *array;
    int size;
    int limit;
} movelist;
typedef movelist *movelist_t;

extern const move NULL_MOVE;

extern const uint8_t M_FLAG_QUIET;
extern const uint8_t M_FLAG_DPP;
extern const uint8_t M_FLAG_CASTLING[2];
extern const uint8_t M_FLAG_CAPTURE;
extern const uint8_t M_FLAG_EN_PASSANT;
extern const uint8_t M_FLAG_PROMOTION[5];

/* Moves Interface */

void move_apply(position_t P, move m);

void move_print(move m);

/* Movelist Interface */

movelist_t movelist_new(void);

int movelist_length(movelist_t M);

void movelist_free(movelist_t M);

movelist_t generate_moves(movelist_t M, position_t P);

void movelist_print(movelist_t M);

#endif
