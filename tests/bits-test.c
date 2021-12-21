/**
 * @file bits-test.c
 * @brief Tests for the bits interface.
 */

#include "../src/bits.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void bits_tests(void) {
    /*Calculating squares given rows and cols */
    assert(square_calculate(0,0) == 0);
    assert(square_calculate(0,5) == 5);
    assert(square_calculate(3,2) == 26);
    assert(square_calculate(7,7) == 63);

    /* Getting rows and cols from squares */
    assert(square_get_rank(42) == 5);
    assert(square_get_file(31) == 7);

    /* Converting squares to singular (1 on bit) bitboards */
    assert(square_to_bitboard(3) == 0b1000U);

    /* Converting squares */
    assert(strcmp(square_to_string(A5), "a5") == 0);
    assert(square_from_string("h3") == H3);

    /* Setting/resetting squares */
    assert(bitboard_set(0b00001, 4) == 0b10001);
    assert(bitboard_reset(0b11000, 3) == 0b10000);

    /* Converting singular bitboard to square */
    assert(bitboard_to_square(0b100000) == 5);

    /* Counting number of (on bits) */
    assert(bitboard_count_bits(0b11010011) == 5);

    /* 180 degree bitboard rotation, aka bit reversal */
    assert(bitboard_rotate(0xABCDEFABCDEFABCD) == 0xB3D5F7B3D5F7B3D5);

    /* Enum */
    assert(A3 == 16);
    return;
}

int main(void) {
    bits_tests();

    /* quick XOR test */
    bitboard b = 0;
    square s = square_calculate(7, 0);
    b |= square_to_bitboard(s);
    bitboard_print(b);

    printf("All tests passed!\n");
    return 0;
}