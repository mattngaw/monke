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

/** @brief Gets the pawns from P->pawns */
static const bitboard pawns_mask = 0x00FFFFFFFFFFFF00;

/** @brief En_passant flags for P->pawns */
static const bitboard our_en_passant_mask = 0xFF00000000000000;
static const bitboard their_en_passant_mask = 0xFF;

/** @brief Castling flags for P->castling */
static const uint8_t our_OO_mask = 0b1000;
static const uint8_t our_OOO_mask = 0b0100;
static const uint8_t their_OO_mask = 0b0010;
static const uint8_t their_OOO_mask = 0b0001;

/** Helper functions */

static bool is_board(position *P) {
    if (P == NULL)
        return false;

    bitboard all = P->ours | P->theirs;
    bitboard pawns = P->pawns & pawns_mask;
    bitboard kings = square_to_bitboard(P->our_king) 
                     | square_to_bitboard(P->their_king);
    bitboard all_pieces = pawns | P->knights | P->bishops | P->rooks 
                          | P->queens | kings;

    bool no_color_overlap = (P->ours & P->theirs) == 0;
    bool no_pieces_overlap = (pawns & P->knights & P->bishops & P->rooks 
                             & P->queens & kings) == 0;
    bool two_kings = bitboard_count_bits(kings & all) == 2;

    return no_color_overlap && no_pieces_overlap && all == all_pieces 
           && two_kings;
}

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
                    P->pawns |= b;
                    break;
                case 'n':
                    P->knights |= b;
                    break;
                case 'b':
                    P->bishops |= b;
                    break;
                case 'r':
                    P->rooks |= b;
                    break;
                case 'q':
                    P->queens |= b;
                    break;
                case 'k':
                    if (is_black) P->their_king = s;
                    else P->our_king = s;
                    break;
                default:
                    printf("Alpha character '%c' is not a piece!", pieces[i]);
                    return;
            }

            if (is_black) P->theirs |= b;
            else P->ours |= b;
            
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

/** Interface functions */

position *position_new(void) {
    position *P = malloc(sizeof(position));
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
    P->ours = P->theirs = P->pawns = P->knights = P->bishops = P->rooks 
              = P->queens = bitboard_empty;
    P->our_king = P->their_king = -1;
    P->halfmoves = P->fullmoves = 0;
    P->castling = 0b0000;
    P->rotated = false;
    return;
}

void position_init(position *P) {
    dbg_requires(P != NULL);
    
    position_from_fen(P, starting_fen);

    dbg_ensures(is_board(P));

    return;
}

void position_from_fen(position *P, char *fen) {
    dbg_requires(P != NULL);
    char *token;
    bool is_black;

    position_clear(P);

    // Pieces
    token = strtok(fen, " ");
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
                    position_set_our_OO(P, true);
                    break;
                case 'Q':
                    position_set_our_OOO(P, true);
                    break;
                case 'k':
                    position_set_their_OO(P, true);
                    break;
                case 'q':
                    position_set_their_OOO(P, true);
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
        P->en_passant = square_from_string(token);

    // Halfmove
    token = strtok(NULL, " ");
    P->halfmoves = strtol(token, NULL, 10);

    // Fullmove
    token = strtok(NULL, " ");
    P->fullmoves = strtol(token, NULL, 10);

    if (is_black) {
        position_rotate(P);
    }
    
    dbg_ensures(is_board(P));
    return;
}

// TODO: IMPLEMENT
// char *position_to_fen(position *P) {
//     dbg_requires(is_board(P));
// }

bitboard position_get_our_pawns(position *P) {
    dbg_requires(is_board(P));
    bitboard our_pawns = P->ours & P->pawns & pawns_mask;
    return our_pawns;
}

void position_set_our_pawns(position *P, bitboard b) {
    dbg_requires(is_board(P));
    dbg_requires((b & pawns_mask) == 0);
    P->ours ^= b;
    P->pawns ^= b;
    return;
}

bitboard position_get_their_pawns(position *P) {
    dbg_requires(is_board(P));
    bitboard their_pawns = P->theirs & P->pawns & pawns_mask;
    return their_pawns;
}

void position_set_their_pawns(position *P, bitboard b) {
    dbg_requires(is_board(P));
    dbg_requires((b & pawns_mask) == 0);
    P->theirs ^= b;
    P->pawns ^= b;
    return;
}

square position_get_our_en_passant(position *P) {
    dbg_requires(is_board(P));
    bitboard our_en_passant = P->pawns & our_en_passant_mask;
    square en_passant_square = bitboard_to_square(our_en_passant) - 16;
    return en_passant_square;
}

void position_set_our_en_passant(position *P, square s) {
    dbg_requires(is_board(P));
    dbg_requires(40 <= s && s < 48);
    P->pawns |= square_to_bitboard(s + 16);
    return;
}

square position_get_their_en_passant(position *P) {
    dbg_requires(is_board(P));
    bitboard their_en_passant = P->pawns & their_en_passant_mask;
    square en_passant_square = bitboard_to_square(their_en_passant) + 16;
    return en_passant_square;
}

void position_set_their_en_passant(position *P, square s) {
    dbg_requires(is_board(P));
    dbg_requires(16 <= s && s < 24);
    P->pawns |= square_to_bitboard(s - 16);
    return;
}

bitboard position_get_our_knights(position *P) {
    dbg_requires(is_board(P));
    bitboard our_knights = P->ours & P->knights;
    return our_knights;
}

void position_set_our_knights(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->ours ^= b;
    P->knights ^= b;
    return;
}

bitboard position_get_their_knights(position *P) {
    dbg_requires(is_board(P));
    bitboard their_knights = P->theirs & P->knights;
    return their_knights;
}

void position_set_their_knights(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->theirs ^= b;
    P->knights ^= b;
    return;
}
 
bitboard position_get_our_bishops(position *P) {
    dbg_requires(is_board(P));
    bitboard our_bishops = P->ours & P->bishops;
    return our_bishops;
}

void position_set_our_bishops(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->ours ^= b; 
    P->bishops ^= b;
    return;
}

bitboard position_get_their_bishops(position *P) {
    dbg_requires(is_board(P));
    bitboard their_bishops = P->theirs & P->bishops;
    return their_bishops;
}

void position_set_their_bishops(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->theirs ^= b;
    P->bishops ^= b;
    return;
}

bitboard position_get_our_rooks(position *P) {
    dbg_requires(is_board(P));
    bitboard our_rooks = P->ours & P->rooks;
    return our_rooks;
}

void position_set_our_rooks(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->ours ^= b;
    P->rooks ^= b;
    return;
}

bitboard position_get_their_rooks(position *P) {
    dbg_requires(is_board(P));
    bitboard their_rooks = P->theirs & P->rooks;
    return their_rooks;
}

void position_set_their_rooks(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->theirs ^= b;
    P->rooks ^= b;
    return;
}

bitboard position_get_our_queens(position *P) {
    dbg_requires(is_board(P));
    bitboard our_queens = P->ours & P->queens;
    return our_queens;
}

void position_set_our_queens(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->ours ^= b;
    P->queens ^= b;
    return;
}

bitboard position_get_their_queens(position *P) {
    dbg_requires(is_board(P));
    bitboard their_queens = P->theirs & P->queens;
    return their_queens;
}

void position_set_their_queens(position *P, bitboard b) {
    dbg_requires(is_board(P));
    P->theirs ^= b;
    P->queens ^= b;
    return;
}

bitboard position_get_our_king(position *P) {
    dbg_requires(is_board(P));
    bitboard our_king = square_to_bitboard(P->our_king);
    return our_king;
}

void position_set_our_king(position *P, square s) {
    dbg_requires(is_board(P));
    P->our_king = s;
    return;
}

bitboard position_get_their_king(position *P) {
    dbg_requires(is_board(P));
    bitboard their_king = square_to_bitboard(P->their_king);
    return their_king;
}

void position_set_their_king(position *P, square s) {
    dbg_requires(is_board(P));
    P->their_king = s;
    return;
}

bool position_get_our_OO(position *P) {
    dbg_requires(is_board(P));
    bool our_OO = P->castling & our_OO_mask;
    return our_OO;
}

void position_set_our_OO(position *P, bool can_OO) {
    dbg_requires(is_board(P));
    P->castling |= our_OO_mask * can_OO;
    return;
}

bool position_get_our_OOO(position *P) {
    dbg_requires(is_board(P));
    bool our_OOO = P->castling & our_OOO_mask;
    return our_OOO;
}

void position_set_our_OOO(position *P, bool can_OOO) {
    dbg_requires(is_board(P));
    P->castling |= our_OOO_mask * can_OOO;
    return;
}

bool position_get_their_OO(position *P) {
    dbg_requires(is_board(P));
    bool their_OO = P->castling & their_OO_mask;
    return their_OO;
}

void position_set_their_OO(position *P, bool can_OO) {
    dbg_requires(is_board(P));
    P->castling |= their_OO_mask * can_OO;
    return;
}

bool position_get_their_OOO(position *P) {
    dbg_requires(is_board(P));
    bool their_OOO = P->castling & their_OOO_mask;
    return their_OOO;
}

void position_set_their_OOO(position *P, bool can_OOO) {
    dbg_requires(is_board(P));
    P->castling |= their_OOO_mask * can_OOO;
    return;
}

void position_rotate(position *P) {
    P->ours = bitboard_rotate(P->ours);
    P->theirs = bitboard_rotate(P->theirs);
    P->pawns = bitboard_rotate(P->pawns);
    P->knights = bitboard_rotate(P->knights);
    P->bishops = bitboard_rotate(P->bishops);
    P->rooks = bitboard_rotate(P->rooks);
    P->queens = bitboard_rotate(P->queens);

    bitboard temp_b = P->ours;
    P->ours = P->theirs;
    P->theirs = temp_b;
    
    bitboard our_king = square_to_bitboard(P->our_king);
    bitboard their_king = square_to_bitboard(P->their_king);
    P->our_king = bitboard_to_square(bitboard_rotate(their_king));
    P->their_king = bitboard_to_square(bitboard_rotate(our_king));

    uint8_t our_castling = P->castling & (our_OO_mask | our_OOO_mask);
    uint8_t their_castling = P->castling & (their_OO_mask | their_OOO_mask);
    P->castling = (our_castling >> 2) | (their_castling << 2);
    
    P->rotated = !P->rotated;
}

void position_print(position *P) {
    char board[8][8];
    bitboard b;
    rank r;
    file f;
    bool is_black;
    for (square s = 0; s < 64; s++) {
        b = square_to_bitboard(s);
        r = square_get_rank(s);
        f = square_get_file(s);

        if (P->ours & b) is_black = false;
        else if (P->theirs & b) is_black = true;
        else {
            board[7 - r][f] = '.';
            continue;
        }

        if (P->pawns & b) board[7 - r][f] = 'P';
        else if (P->knights & b) board[7 - r][f] = 'N';
        else if (P->bishops & b) board[7 - r][f] = 'B';
        else if (P->rooks & b) board[7 - r][f] = 'R';
        else if (P->queens & b) board[7 - r][f] = 'Q';
        else if (P->our_king == s || P->their_king == s) board[7 - r][f] = 'K';
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

    if (P->rotated) printf("Black to move.\n");
    else printf("White to move.\n");

    printf("W: ");
    if (P->castling & our_OO_mask) printf("O-O ");
    if (P->castling & our_OOO_mask) printf("O-O-O ");
    printf("B: ");
    if (P->castling & their_OO_mask) printf("O-O ");
    if (P->castling & their_OOO_mask) printf("O-O-O ");
    printf("\n\n");
}