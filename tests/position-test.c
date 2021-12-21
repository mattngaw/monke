/**
 * @file bits-test.c
 * @brief Tests for the bits interface.
 */

#include "../src/position.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void position_tests(void) {
    position_t P = position_new();

    position_init(P);
    position_print(P);
    P->pawns ^= 0x0000000010001000;
    P->ours ^= 0x0000000010001000;
    bitboard_print(P->pawns);
    position_print(P);

    position_rotate(P);
    position_print(P);

    position_rotate(P);
    position_print(P);

    position_free(P);

    return;
}

int main(int argc, char *argv[]) {
    // char *fen;
    // char *print;
    
    position_tests();
    
    printf("All tests passed!\n");

    // if (argc == 1) {
    //     fen = starting_fen;
    //     print = "board";
    // } else if (argc == 2) {
    //     fen = argv[1];
    //     print = "board";
    // } else if (argc == 3) {
    //     fen = argv[1];
    //     print = argv[2];
    // } else { // argc != 1,2,3
    //     printf("Needs 2 arguments, %d passed.\n", argc - 1);
    //     return 1;
    // }

    // position_t P = position_new();

    // position_from_fen(P, fen);

    // if (strcmp(print, "ours") == 0) {
    //     bitboard_print(P->ours);
    // } else if (strcmp(print, "theirs") == 0) {
    //     bitboard_print(P->theirs);
    // } else if (strcmp(print, "pawns") == 0) {
    //     bitboard_print(P->pawns);
    // } else if (strcmp(print, "knights") == 0) {
    //     bitboard_print(P->knights);
    // } else if (strcmp(print, "bishops") == 0) {
    //     bitboard_print(P->bishops);
    // } else if (strcmp(print, "rooks") == 0) {
    //     bitboard_print(P->rooks);
    // } else if (strcmp(print, "queens") == 0) {
    //     bitboard_print(P->queens);
    // } else if (strcmp(print, "kings") == 0) {
    //     bitboard_print(square_to_bitboard(P->our_king) | square_to_bitboard(P->their_king));
    // } else if (strcmp(print, "board") == 0) {
    //     position_print(P);
    // } else {
    //     printf("Not a recognized command\n");
    // }

    // position_free(P);

    return 0;
}