/**
 * @file zobrist.h
 * @brief Provides an interface for hashing positions positions.
 */

#ifndef _ZOBRIST_H_
#define _ZOBRIST_H_

#include "bits.h"
#include "position.h"

typedef uint64_t zhash;

/** @brief Initializes the seed and pseudo-random numbers for hashing */
void hash_init(void);

/** @brief Returns a hash value for a given position */
zhash hash_position(position *P);

#endif