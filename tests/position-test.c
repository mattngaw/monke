/**
 * @file bits-test.c
 * @brief Tests for the bits interface.
 */

#include "../src/position.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void position_tests(void) {
    position *P = position_new();

    position_init(P);
    position_print(P);
    P->pieces[PAWN] ^= square_to_bitboard(E2) | square_to_bitboard(E4);
    P->whose[OURS] ^= square_to_bitboard(E2) | square_to_bitboard(E4);
    bitboard_print(P->pieces[PAWN]);
    position_print(P);

    position_rotate(P);
    position_print(P);

    position_rotate(P);
    position_set_castling(P, THEIRS, QUEENSIDE, false);
    position_print(P);

    position_free(P);

    return;
}

int main(int argc, char *argv[]) {
    position_tests();
    
    printf("All tests passed!\n");

    return 0;
}