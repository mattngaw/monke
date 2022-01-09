/**
 * @file board.c
 * @brief Implementation of a chess board interface.
 */

#include "position.h"
#include "bits.h"

#include "../lib/contracts.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char PIECE_CHARS[2][6] = {
    { 'P', 'N', 'B', 'R', 'Q', 'K' },
    { 'p', 'n', 'b', 'r', 'q', 'k' }
};

/* Gets the pawns from P->pieces[PAWN] */
static const bitboard PAWNS_MASK = 0x00FFFFFFFFFFFF00;

/* En_passant flags for P->pieces[PAWN] */
static const bitboard EN_PASSANT_MASKS[2] = { 0xFF00000000000000 , 
                                              0x00000000000000FF };

/* Castling flags for P->castling */
static const uint8_t CASTLING_MASKS[2][2] = { {0b1000, 0b0100}, {0b0010, 0b0001} };

static const char STARTING_FEN[] = 
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/*
 * ---------------------------------------------------------------------------
 *                                POSITIONS 
 * ---------------------------------------------------------------------------
 */

/* Helper function for contract assertions */
static bool is_position(position *P) {
    if (P == NULL)
        return false;

    bitboard all = P->whose[OURS] | P->whose[THEIRS];
    bitboard pawns = P->pieces[PAWN] & PAWNS_MASK;
    bitboard kings = square_to_bitboard(P->king[OURS]) 
                     | square_to_bitboard(P->king[THEIRS]);
    bitboard all_pieces = pawns | P->pieces[KNIGHT] | P->pieces[BISHOP] | P->pieces[ROOK] 
                          | P->pieces[QUEEN] | kings;

    bool no_color_overlap = (P->whose[OURS] & P->whose[THEIRS]) == 0;
    bool no_pieces_overlap = (pawns & P->pieces[KNIGHT] & P->pieces[BISHOP] & P->pieces[ROOK] 
                             & P->pieces[QUEEN] & kings) == 0;
    bool two_kings = bitboard_count_bits(kings & all) == 2;

    return no_color_overlap && no_pieces_overlap && all == all_pieces 
           && two_kings;
}

/** Interface functions */

position *position_new(void) {
    position *P = malloc(sizeof(position));
    if (P == NULL) {
        perror("malloc error");
        exit(1);
    }
    position_clear(P);
    dbg_ensures(P != NULL);
    return P;
}

void position_free(position *P) {
    free(P);
    return;
}

void position_clear(position *P) {
    dbg_requires(P != NULL);
    P->whose[OURS] = P->whose[THEIRS] = P->pieces[PAWN] = P->pieces[KNIGHT] = P->pieces[BISHOP] = P->pieces[ROOK] 
              = P->pieces[QUEEN] = BITBOARD_EMPTY;
    P->king[OURS] = P->king[THEIRS] = INVALID_SQUARE;
    P->halfmoves = P->fullmoves = 0;
    P->castling = 0b0000;
    P->color = false;
    return;
}

void position_init(position *P) {
    dbg_requires(P != NULL);
    
    position_from_fen(P, STARTING_FEN);

    dbg_ensures(is_position(P));

    return;
}

/* position_from_fen */

static bool is_alpha(char c) {
    return (0x41 <= c && c <= 0x5A) || (0x61 <= c && c <= 0x7A);
}

static bool is_lower(char c) {
    return 0x61 <= c && c <= 0x7A;
}

static char to_lower(char c) {
    return c + (0x20 * (!is_lower(c)));
}

static void position_from_fen_pieces(position *P, char *pieces) {
    int pieces_len = strlen(pieces);

    rank r = 7;
    file f = 0;
    
    for (int i = 0; i < pieces_len; i++) {
        if (0x31 <= pieces[i] && pieces[i] <= 0x38) {
            f += (uint8_t)(pieces[i] - 0x30);
            dbg_assert(f <= 8);
        } else if (is_alpha(pieces[i])) {
            square s = square_calculate(r, f);
            bitboard b = square_to_bitboard(s);
            bool is_black = is_lower(pieces[i]);

            switch (to_lower(pieces[i])) {
                case 'p':
                    P->pieces[PAWN] |= b;
                    break;
                case 'n':
                    P->pieces[KNIGHT] |= b;
                    break;
                case 'b':
                    P->pieces[BISHOP] |= b;
                    break;
                case 'r':
                    P->pieces[ROOK] |= b;
                    break;
                case 'q':
                    P->pieces[QUEEN] |= b;
                    break;
                case 'k':
                    if (is_black) P->king[THEIRS] = s;
                    else P->king[OURS] = s;
                    break;
                default:
                    printf("Alpha character '%c' is not a piece!", pieces[i]);
                    return;
            }

            if (is_black) P->whose[THEIRS] |= b;
            else P->whose[OURS] |= b;
            
            f++;

            dbg_assert(f <= 8);
        } else if (pieces[i] == '/') {
            dbg_assert(f == 8);

            f = 0;
            r--;
        } else {
            printf("Invalid character:'%c'", pieces[i]);
            return;
        }
    }
}

static bool position_from_fen_side(position *P, char *side) {
    int side_len = strlen(side);

    if (side_len != 1) {
        printf("Invalid char: %c", side[0]);
        exit(1);
    } 

    if (side[0] == 'w') {
        return false;
    } else if (side[0] == 'b') {
        return true;
    }
    exit(1);
}

void position_from_fen(position *P, const char *fen) {
    dbg_requires(P != NULL);
    char *token, *temp;
    bool is_black;

    temp = malloc(sizeof(char) * (strlen(fen)+1));
    temp = strncpy(temp, fen, strlen(fen));

    position_clear(P);

    // Pieces
    token = strtok(temp, " ");
    position_from_fen_pieces(P, token);
    
    // Side to move
    token = strtok(NULL, " ");
    int side_len = strlen(token);

    if (side_len != 1) {
        printf("Invalid FEN side-to-move: %s", token);
        exit(1);
    }

    if (token[0] == 'w') {
        is_black = false;
    } else if (token[0] == 'b') {
        is_black = true;
    } else {
        printf("Invalid FEN side-to-move char: %c", token[0]);
        exit(1);
    }
    // Castling
    token = strtok(NULL, " ");
    int castling_len = strlen(token);

    if (!(castling_len == 1 && token[0] == '-')) {
        if (!(1 <= castling_len && castling_len <= 4)) {
            printf("Castling string (%s) too long\n", token);
            exit(1);
        }
        for (int i = 0; i < castling_len; i++) {
            switch (token[i] ) {
                case 'K':
                    position_set_castling(P, OURS, KINGSIDE, true);
                    break;
                case 'Q':
                    position_set_castling(P, OURS, QUEENSIDE, true);
                    break;
                case 'k':
                    position_set_castling(P, THEIRS, KINGSIDE, true);
                    break;
                case 'q':
                    position_set_castling(P, THEIRS, QUEENSIDE, true);
                    break;
                default:
                    printf("Invalid castling char (%c)\n", token[i]);
                    exit(1);
            }
        }
    }

    // En passant flag
    token = strtok(NULL, " ");
    if (token[0] != '-')
        return;

    // Halfmove
    token = strtok(NULL, " ");
    P->halfmoves = strtol(token, NULL, 10);

    // Fullmove
    token = strtok(NULL, " ");
    P->fullmoves = strtol(token, NULL, 10);

    if (is_black) {
        position_rotate(P);
    }
    
    dbg_ensures(is_position(P));
    return;
}

/* [TODO]
char *position_to_fen(position *P);
*/

bitboard position_get_pieces(position_t P, Whose whose, Piece piece) {
    dbg_requires(is_position(P));
    bitboard b = P->whose[whose] & P->pieces[piece];
    return b;
}

void position_set_pieces(position_t P, Whose whose, Piece piece, bitboard b) {
    dbg_requires(is_position(P));
    P->whose[whose] ^= b;
    P->pieces[piece] ^= b;
    return;
}

square position_get_en_passant(position_t P, Whose whose) {
    dbg_requires(is_position(P));
    square offset = whose ? -16 : 16; // -16 if OURS, 16 if THEIRS
    bitboard en_passant_bb = P->pieces[PAWN] & EN_PASSANT_MASKS[whose];
    return bitboard_to_square(en_passant_bb) + offset;
}

void position_set_en_passant(position_t P, Whose whose, square to) {
    dbg_requires(is_position(P));
    square s;
    if (whose == OURS)
        s = to - 24;
    else // whose == THEIRS
        s = to + 24;
    P->pieces[PAWN] |= square_to_bitboard(s);
    return;
}

void position_reset_en_passant(position_t P) {
    P->pieces[PAWN] &= PAWNS_MASK;
    return;
}

bitboard position_get_king(position_t P, Whose whose) {
    dbg_requires(is_position(P));
    bitboard b = square_to_bitboard(P->king[whose]);
    return b;
}

void position_set_king(position_t P, Whose whose, square s) {
    P->whose[OURS] ^= square_to_bitboard(P->king[whose]) | square_to_bitboard(s);
    P->king[whose] = s;
    return;
}

bool position_get_castling(position_t P, Whose whose, Castling castling) {
    bool can_castle = P->castling & CASTLING_MASKS[whose][castling];
    return can_castle;
}

void position_set_castling(position_t P, Whose whose, Castling castling, bool can_castle) {
    P->castling = (P->castling & ~CASTLING_MASKS[whose][castling]) | 
                  (CASTLING_MASKS[whose][castling] * can_castle);
    return;
}

void position_rotate(position *P) {
    P->whose[OURS] = bitboard_rotate(P->whose[OURS]);
    P->whose[THEIRS] = bitboard_rotate(P->whose[THEIRS]);
    P->pieces[PAWN] = bitboard_rotate(P->pieces[PAWN]);
    P->pieces[KNIGHT] = bitboard_rotate(P->pieces[KNIGHT]);
    P->pieces[BISHOP] = bitboard_rotate(P->pieces[BISHOP]);
    P->pieces[ROOK] = bitboard_rotate(P->pieces[ROOK]);
    P->pieces[QUEEN] = bitboard_rotate(P->pieces[QUEEN]);

    bitboard temp_b = P->whose[OURS];
    P->whose[OURS] = P->whose[THEIRS];
    P->whose[THEIRS] = temp_b;
    
    bitboard our_king = square_to_bitboard(P->king[OURS]);
    bitboard their_king = square_to_bitboard(P->king[THEIRS]);
    P->king[OURS] = bitboard_to_square(bitboard_rotate(their_king));
    P->king[THEIRS] = bitboard_to_square(bitboard_rotate(our_king));

    uint8_t our_castling = P->castling & (CASTLING_MASKS[OURS][KINGSIDE] | CASTLING_MASKS[OURS][QUEENSIDE]);
    uint8_t their_castling = P->castling & (CASTLING_MASKS[THEIRS][KINGSIDE] | CASTLING_MASKS[THEIRS][QUEENSIDE]);
    P->castling = (our_castling >> 2) | (their_castling << 2);
    
    P->color = !P->color;
}

void position_print(position *P) {
    dbg_requires(is_position(P));
    char board[8][8];
    bitboard b;
    rank r;
    file f;
    bool is_black;
    for (square s = 0; s < 64; s++) {
        b = square_to_bitboard(s);
        r = square_get_rank(s);
        f = square_get_file(s);

        if (P->whose[OURS] & b) is_black = false;
        else if (P->whose[THEIRS] & b) is_black = true;
        else {
            board[7 - r][f] = '.';
            continue;
        }

        if (P->pieces[PAWN] & b) board[7 - r][f] = 'P';
        else if (P->pieces[KNIGHT] & b) board[7 - r][f] = 'N';
        else if (P->pieces[BISHOP] & b) board[7 - r][f] = 'B';
        else if (P->pieces[ROOK] & b) board[7 - r][f] = 'R';
        else if (P->pieces[QUEEN] & b) board[7 - r][f] = 'Q';
        else if (P->king[OURS] == s || P->king[THEIRS] == s) board[7 - r][f] = 'K';
        else {
            printf("position_print error\n");
            exit(1);
        }

        if (is_black) board[7 - r][f] += 0x20;
    }
    printf("CURRENT POSITION:\n");
    for (r = 0; r < 8; r++) {
        for (f = 0; f < 8; f++) {
            printf("%c ", board[r][f]);
        }
        printf("\n");
    }

    if (P->color) printf("Black to move.\n");
    else printf("White to move.\n");

    printf("W: ");
    if (P->castling & CASTLING_MASKS[OURS][KINGSIDE]) printf("O-O ");
    if (P->castling & CASTLING_MASKS[OURS][QUEENSIDE]) printf("O-O-O ");
    printf("B: ");
    if (P->castling & CASTLING_MASKS[THEIRS][KINGSIDE]) printf("O-O ");
    if (P->castling & CASTLING_MASKS[THEIRS][QUEENSIDE]) printf("O-O-O ");
    printf("\n\n");
}