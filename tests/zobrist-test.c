/**
 * @file zobrist-test.c
 * @brief Tests for the zobrist hashing interface.
 */

#include "../src/moves.h"
#include "../src/zobrist.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void zobrist_tests(void) {
    
    position *P = position_new();
    position_clear(P);
    hash_init();
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));
    
    position_init(P);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));

    move m0 = {PAWN, E2, E3, M_FLAG_QUIET};
    move_make(P, m0);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));
    position_rotate(P);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));

    move m1 = {PAWN, d7, d5, M_FLAG_DPP};
    move_make(P, m1);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));
    position_set_castling(P, OURS, KINGSIDE, false);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));

    position_rotate(P);
    move m2 = {KING, E1, E2, M_FLAG_QUIET};
    move_make(P, m2);
    position_print(P);
    assert(hash_position(P) == hash_position(P));
    printf("Zobrist hash: %lu\n", hash_position(P));
    
    position_free(P);

    return;
}

int main(int argc, char *argv[]) {
    zobrist_tests();
    
    printf("All tests passed!\n");

    return 0;
}