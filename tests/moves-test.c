/**
 * @file moves-test.c
 * @brief Tests for the moves interface.
 */

#include "../src/moves.h"
#include "../src/position.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void moves_tests(void) {
    position_t P = position_new();
    position_init(P);

    P->whose[OURS] ^= square_to_bitboard(E2) | square_to_bitboard(E4);
    P->pieces[PAWN] ^= square_to_bitboard(E2) | square_to_bitboard(E4);

    P->whose[OURS] ^= square_to_bitboard(E4) | square_to_bitboard(E5);
    P->pieces[PAWN] ^= square_to_bitboard(E4) | square_to_bitboard(E5);

    P->whose[OURS] ^= square_to_bitboard(E5) | square_to_bitboard(E6);
    P->pieces[PAWN] ^= square_to_bitboard(E5) | square_to_bitboard(E6);

    P->whose[OURS] ^= square_to_bitboard(D2) | square_to_bitboard(D4);
    P->pieces[PAWN] ^= square_to_bitboard(D2) | square_to_bitboard(D4);

    P->whose[OURS] ^= square_to_bitboard(F1) | square_to_bitboard(C4);
    P->pieces[BISHOP] ^= square_to_bitboard(F1) | square_to_bitboard(C4);

    P->whose[OURS] ^= square_to_bitboard(G1) | square_to_bitboard(F3);
    P->pieces[KNIGHT] ^= square_to_bitboard(G1) | square_to_bitboard(F3);
    position_print(P);

    movelist_t M = movelist_new();
    generate_moves(M, P);
    movelist_print(M);

    move_apply(P, M->array[10]);
    position_print(P);

    movelist_free(M);
    position_free(P);

    return;
}

int main(void) { 
    moves_tests();

    printf("All tests passed!\n");
    
    return 0;
}