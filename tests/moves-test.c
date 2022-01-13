/**
 * @file moves-test.c
 * @brief Tests for the moves interface.
 */

#include "../src/position.h"
#include "../src/moves.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static bool is_numerical(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if ('0' > str[i] || str[i] > '9') return false;
    }
    return true;
}

static int randrange(int lower, int upper) {
    int n;
    srand(time(NULL));
    printf("%d\n", (n = rand()));
    return (n % (upper - lower + 1)) + lower;
}

void moves_tests(void) {
    char s[20];
    move m;
    GameState G;
    position *P = position_new();
    position_clear(P);
    position_from_fen(P, "8/8/8/8/k7/4r3/2r5/K7 b - - 0 1");

    movelist_t M = movelist_new();
    
    while(true) {
        printf("\n");
        position_print(P);
        movelist_clear(M);
        generate_moves(M, P);

        G = get_game_state(P, M);
        if (G == CHECKMATE) {
            printf("Checkmate!\n");
            break;
        } else if (G == DRAW) {
            printf("Draw!\n");
            break;
        } // G == CONTINUE

        if (king_in_check(P, OURS)) printf("Check!\n");

        printf("> ");
        scanf("%s", s);
        if (is_numerical(s) && atoi(s) < M->size) {
            m = M->array[atoi(s)];
            printf("%d: ", atoi(s));
            move_print(m, P->color);
            move_make(P, m);
            position_rotate(P);
        } else if (strcmp(s, "random") == 0) {
            int k = randrange(0, M->size-1);
            m = M->array[k];
            printf("%d: ", k);
            move_print(m, P->color);
            move_make(P, m);
            position_rotate(P);
        } else if (strcmp(s, "print") == 0) {
            movelist_print(M, P->color);
        } else if (strcmp(s, "quit") == 0) {
            printf("Quitting!\n\n");
            break;
        } else {
            printf("Invalid response\n");
        }
    }

    position_free(P);

    return;
}

int main(void) { 
    moves_tests();

    printf("All tests passed!\n");
    
    return 0;
}