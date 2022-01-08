/**
 * @file moves.c
 * @brief Provides an interface for generating and applying moves.
 */

#include "bits.h"
#include "position.h"
#include "moves.h"

#include "../lib/contracts.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_EAST,
    SOUTH_EAST,
    SOUTH_WEST,
    NORTH_WEST
} Direction;

const move NULL_MOVE = {0};

const uint8_t M_FLAG_QUIET = 0x00;
const uint8_t M_FLAG_DPP = 0x01;
const uint8_t M_FLAG_CASTLING[2] = { 0x02, 0x03 };
const uint8_t M_FLAG_CAPTURE = 0x04;
const uint8_t M_FLAG_EN_PASSANT = 0x05; // Includes M_FLAG_CAPTURE
const uint8_t M_FLAG_PROMOTION[5] = { 0x00, 0x08, 0x09, 0x0A, 0x0B };

static const bitboard CASTLING_MASK[2] = { 0x60, 0x0C };

static const bitboard RAYS[8][64] = {
    {   /* NORTH */
        0x0101010101010100, 0x0202020202020200, 0x0404040404040400,
        0x0808080808080800, 0x1010101010101000, 0x2020202020202000,
        0x4040404040404000, 0x8080808080808000, 0x0101010101010000,
        0x0202020202020000, 0x0404040404040000, 0x0808080808080000,
        0x1010101010100000, 0x2020202020200000, 0x4040404040400000,
        0x8080808080800000, 0x0101010101000000, 0x0202020202000000,
        0x0404040404000000, 0x0808080808000000, 0x1010101010000000,
        0x2020202020000000, 0x4040404040000000, 0x8080808080000000,
        0x0101010100000000, 0x0202020200000000, 0x0404040400000000,
        0x0808080800000000, 0x1010101000000000, 0x2020202000000000,
        0x4040404000000000, 0x8080808000000000, 0x0101010000000000,
        0x0202020000000000, 0x0404040000000000, 0x0808080000000000,
        0x1010100000000000, 0x2020200000000000, 0x4040400000000000,
        0x8080800000000000, 0x0101000000000000, 0x0202000000000000,
        0x0404000000000000, 0x0808000000000000, 0x1010000000000000,
        0x2020000000000000, 0x4040000000000000, 0x8080000000000000,
        0x0100000000000000, 0x0200000000000000, 0x0400000000000000,
        0x0800000000000000, 0x1000000000000000, 0x2000000000000000,
        0x4000000000000000, 0x8000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000
    },
    {   /* EAST */
        0x00000000000000fe, 0x00000000000000fc, 0x00000000000000f8,
        0x00000000000000f0, 0x00000000000000e0, 0x00000000000000c0,
        0x0000000000000080, 0x0000000000000000, 0x000000000000fe00,
        0x000000000000fc00, 0x000000000000f800, 0x000000000000f000,
        0x000000000000e000, 0x000000000000c000, 0x0000000000008000,
        0x0000000000000000, 0x0000000000fe0000, 0x0000000000fc0000,
        0x0000000000f80000, 0x0000000000f00000, 0x0000000000e00000,
        0x0000000000c00000, 0x0000000000800000, 0x0000000000000000,
        0x00000000fe000000, 0x00000000fc000000, 0x00000000f8000000,
        0x00000000f0000000, 0x00000000e0000000, 0x00000000c0000000,
        0x0000000080000000, 0x0000000000000000, 0x000000fe00000000,
        0x000000fc00000000, 0x000000f800000000, 0x000000f000000000,
        0x000000e000000000, 0x000000c000000000, 0x0000008000000000,
        0x0000000000000000, 0x0000fe0000000000, 0x0000fc0000000000,
        0x0000f80000000000, 0x0000f00000000000, 0x0000e00000000000,
        0x0000c00000000000, 0x0000800000000000, 0x0000000000000000,
        0x00fe000000000000, 0x00fc000000000000, 0x00f8000000000000,
        0x00f0000000000000, 0x00e0000000000000, 0x00c0000000000000,
        0x0080000000000000, 0x0000000000000000, 0xfe00000000000000,
        0xfc00000000000000, 0xf800000000000000, 0xf000000000000000,
        0xe000000000000000, 0xc000000000000000, 0x8000000000000000,
        0x0000000000000000
    },
    {   /* SOUTH */
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000001,
        0x0000000000000002, 0x0000000000000004, 0x0000000000000008,
        0x0000000000000010, 0x0000000000000020, 0x0000000000000040,
        0x0000000000000080, 0x0000000000000101, 0x0000000000000202,
        0x0000000000000404, 0x0000000000000808, 0x0000000000001010,
        0x0000000000002020, 0x0000000000004040, 0x0000000000008080,
        0x0000000000010101, 0x0000000000020202, 0x0000000000040404,
        0x0000000000080808, 0x0000000000101010, 0x0000000000202020,
        0x0000000000404040, 0x0000000000808080, 0x0000000001010101,
        0x0000000002020202, 0x0000000004040404, 0x0000000008080808,
        0x0000000010101010, 0x0000000020202020, 0x0000000040404040,
        0x0000000080808080, 0x0000000101010101, 0x0000000202020202,
        0x0000000404040404, 0x0000000808080808, 0x0000001010101010,
        0x0000002020202020, 0x0000004040404040, 0x0000008080808080,
        0x0000010101010101, 0x0000020202020202, 0x0000040404040404,
        0x0000080808080808, 0x0000101010101010, 0x0000202020202020,
        0x0000404040404040, 0x0000808080808080, 0x0001010101010101,
        0x0002020202020202, 0x0004040404040404, 0x0008080808080808,
        0x0010101010101010, 0x0020202020202020, 0x0040404040404040,
        0x0080808080808080
    },
    {   /* WEST */
        0x0000000000000000, 0x0000000000000001, 0x0000000000000003,
        0x0000000000000007, 0x000000000000000f, 0x000000000000001f,
        0x000000000000003f, 0x000000000000007f, 0x0000000000000000,
        0x0000000000000100, 0x0000000000000300, 0x0000000000000700,
        0x0000000000000f00, 0x0000000000001f00, 0x0000000000003f00,
        0x0000000000007f00, 0x0000000000000000, 0x0000000000010000,
        0x0000000000030000, 0x0000000000070000, 0x00000000000f0000,
        0x00000000001f0000, 0x00000000003f0000, 0x00000000007f0000,
        0x0000000000000000, 0x0000000001000000, 0x0000000003000000,
        0x0000000007000000, 0x000000000f000000, 0x000000001f000000,
        0x000000003f000000, 0x000000007f000000, 0x0000000000000000,
        0x0000000100000000, 0x0000000300000000, 0x0000000700000000,
        0x0000000f00000000, 0x0000001f00000000, 0x0000003f00000000,
        0x0000007f00000000, 0x0000000000000000, 0x0000010000000000,
        0x0000030000000000, 0x0000070000000000, 0x00000f0000000000,
        0x00001f0000000000, 0x00003f0000000000, 0x00007f0000000000,
        0x0000000000000000, 0x0001000000000000, 0x0003000000000000,
        0x0007000000000000, 0x000f000000000000, 0x001f000000000000,
        0x003f000000000000, 0x007f000000000000, 0x0000000000000000,
        0x0100000000000000, 0x0300000000000000, 0x0700000000000000,
        0x0f00000000000000, 0x1f00000000000000, 0x3f00000000000000,
        0x7f00000000000000
    },
    {   /* NORTHEAST */
        0x8040201008040200, 0x0080402010080400, 0x0000804020100800,
        0x0000008040201000, 0x0000000080402000, 0x0000000000804000,
        0x0000000000008000, 0x0000000000000000, 0x4020100804020000,
        0x8040201008040000, 0x0080402010080000, 0x0000804020100000,
        0x0000008040200000, 0x0000000080400000, 0x0000000000800000,
        0x0000000000000000, 0x2010080402000000, 0x4020100804000000,
        0x8040201008000000, 0x0080402010000000, 0x0000804020000000,
        0x0000008040000000, 0x0000000080000000, 0x0000000000000000,
        0x1008040200000000, 0x2010080400000000, 0x4020100800000000,
        0x8040201000000000, 0x0080402000000000, 0x0000804000000000,
        0x0000008000000000, 0x0000000000000000, 0x0804020000000000,
        0x1008040000000000, 0x2010080000000000, 0x4020100000000000,
        0x8040200000000000, 0x0080400000000000, 0x0000800000000000,
        0x0000000000000000, 0x0402000000000000, 0x0804000000000000,
        0x1008000000000000, 0x2010000000000000, 0x4020000000000000,
        0x8040000000000000, 0x0080000000000000, 0x0000000000000000,
        0x0200000000000000, 0x0400000000000000, 0x0800000000000000,
        0x1000000000000000, 0x2000000000000000, 0x4000000000000000,
        0x8000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000
    },
    {   /* SOUTHEAST */
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000002,
        0x0000000000000004, 0x0000000000000008, 0x0000000000000010,
        0x0000000000000020, 0x0000000000000040, 0x0000000000000080,
        0x0000000000000000, 0x0000000000000204, 0x0000000000000408,
        0x0000000000000810, 0x0000000000001020, 0x0000000000002040,
        0x0000000000004080, 0x0000000000008000, 0x0000000000000000,
        0x0000000000020408, 0x0000000000040810, 0x0000000000081020,
        0x0000000000102040, 0x0000000000204080, 0x0000000000408000,
        0x0000000000800000, 0x0000000000000000, 0x0000000002040810,
        0x0000000004081020, 0x0000000008102040, 0x0000000010204080,
        0x0000000020408000, 0x0000000040800000, 0x0000000080000000,
        0x0000000000000000, 0x0000000204081020, 0x0000000408102040,
        0x0000000810204080, 0x0000001020408000, 0x0000002040800000,
        0x0000004080000000, 0x0000008000000000, 0x0000000000000000,
        0x0000020408102040, 0x0000040810204080, 0x0000081020408000,
        0x0000102040800000, 0x0000204080000000, 0x0000408000000000,
        0x0000800000000000, 0x0000000000000000, 0x0002040810204080,
        0x0004081020408000, 0x0008102040800000, 0x0010204080000000,
        0x0020408000000000, 0x0040800000000000, 0x0080000000000000,
        0x0000000000000000
    },
    {   /* SOUTHWEST */
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000001, 0x0000000000000002, 0x0000000000000004,
        0x0000000000000008, 0x0000000000000010, 0x0000000000000020,
        0x0000000000000040, 0x0000000000000000, 0x0000000000000100,
        0x0000000000000201, 0x0000000000000402, 0x0000000000000804,
        0x0000000000001008, 0x0000000000002010, 0x0000000000004020,
        0x0000000000000000, 0x0000000000010000, 0x0000000000020100,
        0x0000000000040201, 0x0000000000080402, 0x0000000000100804,
        0x0000000000201008, 0x0000000000402010, 0x0000000000000000,
        0x0000000001000000, 0x0000000002010000, 0x0000000004020100,
        0x0000000008040201, 0x0000000010080402, 0x0000000020100804,
        0x0000000040201008, 0x0000000000000000, 0x0000000100000000,
        0x0000000201000000, 0x0000000402010000, 0x0000000804020100,
        0x0000001008040201, 0x0000002010080402, 0x0000004020100804,
        0x0000000000000000, 0x0000010000000000, 0x0000020100000000,
        0x0000040201000000, 0x0000080402010000, 0x0000100804020100,
        0x0000201008040201, 0x0000402010080402, 0x0000000000000000,
        0x0001000000000000, 0x0002010000000000, 0x0004020100000000,
        0x0008040201000000, 0x0010080402010000, 0x0020100804020100,
        0x0040201008040201
    },
    {   /* NORTHWEST */
        0x0000000000000000, 0x0000000000000100, 0x0000000000010200,
        0x0000000001020400, 0x0000000102040800, 0x0000010204081000,
        0x0001020408102000, 0x0102040810204000, 0x0000000000000000,
        0x0000000000010000, 0x0000000001020000, 0x0000000102040000,
        0x0000010204080000, 0x0001020408100000, 0x0102040810200000,
        0x0204081020400000, 0x0000000000000000, 0x0000000001000000,
        0x0000000102000000, 0x0000010204000000, 0x0001020408000000,
        0x0102040810000000, 0x0204081020000000, 0x0408102040000000,
        0x0000000000000000, 0x0000000100000000, 0x0000010200000000,
        0x0001020400000000, 0x0102040800000000, 0x0204081000000000,
        0x0408102000000000, 0x0810204000000000, 0x0000000000000000,
        0x0000010000000000, 0x0001020000000000, 0x0102040000000000,
        0x0204080000000000, 0x0408100000000000, 0x0810200000000000,
        0x1020400000000000, 0x0000000000000000, 0x0001000000000000,
        0x0102000000000000, 0x0204000000000000, 0x0408000000000000,
        0x0810000000000000, 0x1020000000000000, 0x2040000000000000,
        0x0000000000000000, 0x0100000000000000, 0x0200000000000000,
        0x0400000000000000, 0x0800000000000000, 0x1000000000000000,
        0x2000000000000000, 0x4000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000
    }
};

static const bitboard KNIGHT_ATTACKS[64] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000a1100,
    0x0000000000142200, 0x0000000000284400, 0x0000000000508800,
    0x0000000000a01000, 0x0000000000402000, 0x0000000002040004,
    0x0000000005080008, 0x000000000a110011, 0x0000000014220022,
    0x0000000028440044, 0x0000000050880088, 0x00000000a0100010,
    0x0000000040200020, 0x0000000204000402, 0x0000000508000805,
    0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428,
    0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040,
    0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00,
    0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
    0x0000a0100010a000, 0x0000402000204000, 0x0002040004020000,
    0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000,
    0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000,
    0x0040200020400000, 0x0204000402000000, 0x0508000805000000,
    0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000,
    0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
    0x0400040200000000, 0x0800080500000000, 0x1100110a00000000,
    0x2200221400000000, 0x4400442800000000, 0x8800885000000000,
    0x100010a000000000, 0x2000204000000000, 0x0004020000000000,
    0x0008050000000000, 0x00110a0000000000, 0x0022140000000000,
    0x0044280000000000, 0x0088500000000000, 0x0010a00000000000,
    0x0020400000000000
};

static const bitboard KING_ATTACKS[64] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a,
    0x0000000000001c14, 0x0000000000003828, 0x0000000000007050,
    0x000000000000e0a0, 0x000000000000c040, 0x0000000000030203,
    0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c,
    0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0,
    0x0000000000c040c0, 0x0000000003020300, 0x0000000007050700,
    0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800,
    0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000,
    0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000,
    0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000,
    0x000000e0a0e00000, 0x000000c040c00000, 0x0000030203000000,
    0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000,
    0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000,
    0x0000c040c0000000, 0x0003020300000000, 0x0007050700000000,
    0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000,
    0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000,
    0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000,
    0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000,
    0xe0a0e00000000000, 0xc040c00000000000, 0x0203000000000000,
    0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000,
    0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000,
    0x40c0000000000000
};

static const bitboard PAWN_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000020000,
    0x0000000000050000, 0x00000000000a0000, 0x0000000000140000,
    0x0000000000280000, 0x0000000000500000, 0x0000000000a00000,
    0x0000000000400000, 0x0000000002000000, 0x0000000005000000,
    0x000000000a000000, 0x0000000014000000, 0x0000000028000000,
    0x0000000050000000, 0x00000000a0000000, 0x0000000040000000,
    0x0000000200000000, 0x0000000500000000, 0x0000000a00000000,
    0x0000001400000000, 0x0000002800000000, 0x0000005000000000,
    0x000000a000000000, 0x0000004000000000, 0x0000020000000000,
    0x0000050000000000, 0x00000a0000000000, 0x0000140000000000,
    0x0000280000000000, 0x0000500000000000, 0x0000a00000000000,
    0x0000400000000000, 0x0002000000000000, 0x0005000000000000,
    0x000a000000000000, 0x0014000000000000, 0x0028000000000000,
    0x0050000000000000, 0x00a0000000000000, 0x0040000000000000,
    0x0200000000000000, 0x0500000000000000, 0x0a00000000000000,
    0x1400000000000000, 0x2800000000000000, 0x5000000000000000,
    0xa000000000000000, 0x4000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000
};

static const bitboard PAWN_MOVES[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000001010000,
    0x0000000002020000, 0x0000000004040000, 0x0000000008080000,
    0x0000000010100000, 0x0000000020200000, 0x0000000040400000,
    0x0000000080800000, 0x0000000001000000, 0x0000000002000000,
    0x0000000004000000, 0x0000000008000000, 0x0000000010000000,
    0x0000000020000000, 0x0000000040000000, 0x0000000080000000,
    0x0000000100000000, 0x0000000200000000, 0x0000000400000000,
    0x0000000800000000, 0x0000001000000000, 0x0000002000000000,
    0x0000004000000000, 0x0000008000000000, 0x0000010000000000,
    0x0000020000000000, 0x0000040000000000, 0x0000080000000000,
    0x0000100000000000, 0x0000200000000000, 0x0000400000000000,
    0x0000800000000000, 0x0001000000000000, 0x0002000000000000,
    0x0004000000000000, 0x0008000000000000, 0x0010000000000000,
    0x0020000000000000, 0x0040000000000000, 0x0080000000000000,
    0x0100000000000000, 0x0200000000000000, 0x0400000000000000,
    0x0800000000000000, 0x1000000000000000, 0x2000000000000000,
    0x4000000000000000, 0x8000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000
};


/*
 * ---------------------------------------------------------------------------
 *                                   MOVES
 * ---------------------------------------------------------------------------
 */

static bool move_is_null(move m) {
    return m.piece == 0 && m.from == 0 && m.to == 0 && m.flags == 0;
}

void move_apply(position_t P, move m) {
    bitboard from_bb = square_to_bitboard(m.from);
    bitboard to_bb = square_to_bitboard(m.to);
    bitboard move_bb = from_bb | to_bb;;

    P->whose[OURS] ^= move_bb;
    P->pieces[m.piece] ^= from_bb;

    if (m.flags & M_FLAG_CAPTURE) {
        P->whose[THEIRS] ^= to_bb;
        for (Piece piece = PAWN; piece <= KING; piece++) {
            if (P->pieces[piece] & to_bb) {
                P->pieces[piece] ^= to_bb;
                break;
            }
        }
    }

    P->pieces[m.piece] ^= to_bb;
    return;
}

void move_print(move m) {
    if (m.flags == M_FLAG_CASTLING[KINGSIDE]) {
        printf("O-O\n");
        return;
    }
    if (m.flags == M_FLAG_CASTLING[QUEENSIDE]) {
        printf("O-O-O\n");
        return;
    }
    
    char piece_char = PIECE_CHARS[OURS][m.piece];
    const char *from = SQUARES_TO_STRINGS[m.from];
    const char *to = SQUARES_TO_STRINGS[m.to];

    printf("%c", piece_char);
    printf("%s", from);
    if (m.flags & M_FLAG_CAPTURE) printf("x");
    printf("%s:\n", to);

    char board[8][8];
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            if (square_calculate(r, f) == m.to) board[r][f] = 'T';
            else if (square_calculate(r, f) == m.from) board[r][f] = 'F';
            else board[r][f] = '.';
        }
    }
    for (int r = 8 - 1; r >= 0; r--) {
        for (int f = 0; f < 8; f++) {
            printf("%c ", board[r][f]);
        }
        printf("\n");
    }
    printf("\n");

    return;
}

/*
 * ---------------------------------------------------------------------------
 *                                 MOVELIST
 * ---------------------------------------------------------------------------
 */
// [DONE]
movelist *movelist_new(void) {
    movelist *M = malloc(sizeof(movelist));
    if (M == NULL) {
        perror("malloc error");
        exit(1);
    }
    M->array = malloc(sizeof(move));
    if (M == NULL) {
        perror("malloc error");
        exit(1);
    }
    M->size = 0;
    M->limit = 1;

    return M;
}
// [DONE]
int movelist_length(movelist *M) {
    return M->size;
}
// [DONE]
void movelist_free(movelist *M) {
    free(M->array);
    free(M);
    return;
}
// [[DONE]]
static void movelist_resize(movelist *M, int new_limit) {
    dbg_requires(new_limit >= M->size);

    move *old_array = M->array;
    M->limit = new_limit;
    M->array = malloc(sizeof(move) * new_limit);

    for (int i = 0; i < M->size; i++) {
        M->array[i] = old_array[i];
    }

    free(old_array);

    return;
}
// [DONE]
static void movelist_append(movelist *M, move m) {
    if (M->size == M->limit-1)
        movelist_resize(M, M->limit * 2);

    M->array[M->size++] = m;

    return;
}
// [REVIEW]
static move movelist_pop(movelist *M) {
    if (M->limit == 1)
        return NULL_MOVE;
    
    int half_limit = M->limit / 2;
    if (M->size == half_limit)
        movelist_resize(M, half_limit);
    
    return M->array[M->size--];
}

void movelist_print(movelist_t M) {
    for (int i = 0; i < M->size; i++) {
        printf("%d - ", i);
        move_print(M->array[i]);
    }
    return;
}

/*
 * ---------------------------------------------------------------------------
 *                                 MOVE GEN
 * ---------------------------------------------------------------------------
 */

// [TODO]
static bitboard build_attack_map(position_t P, Whose whose) {
    return BITBOARD_EMPTY;
}
// [DONE]
static bitboard get_pawn_attack_map(square from, Whose whose, 
                                           position_t P) {
    return PAWN_ATTACKS[from] & P->whose[THEIRS];
}
static bitboard get_pawn_quiet_moves_map(square from, Whose whose, 
                                                position_t P) {
    bitboard all = P->whose[OURS] | P->whose[THEIRS];
    return PAWN_MOVES[from] & ~all & ~((all & ~square_to_bitboard(from)) << 8);
}
// [REVIEW]
static movelist *generate_pawn_moves(movelist *M, square from, position_t P) {
    square to;
    bitboard captures = get_pawn_attack_map(from, OURS, P);
    
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { PAWN, from, to, M_FLAG_CAPTURE };
        if (56 <= to && to < 64) {
            for (Piece p = KNIGHT; p <= QUEEN; p++) {
                m.flags |= M_FLAG_PROMOTION[p];
                movelist_append(M, m);
                m.flags &= ~M_FLAG_PROMOTION[p];
            }
        } else {
            movelist_append(M, m);
        }
    }

    bitboard quiet_moves = get_pawn_quiet_moves_map(from, OURS, P);
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { PAWN, from, to, M_FLAG_QUIET };
        if (56 <= to && to < 64) {
            for (Piece p = KNIGHT; p <= QUEEN; p++) {
                m.flags |= M_FLAG_PROMOTION[p];
                movelist_append(M, m);
                m.flags &= ~M_FLAG_PROMOTION[p];
            }
        } else {
            if (to - from == 16)
                m.flags |= M_FLAG_DPP;
            movelist_append(M, m);
        }
    }

    square ep_square = position_get_en_passant(P, OURS);
    if (ep_square != INVALID_SQUARE) {
        bitboard ep_bitboard = square_to_bitboard(ep_square);
        if (captures & ep_bitboard) {
            move m = { PAWN, from, ep_square - 16, M_FLAG_EN_PASSANT };
            movelist_append(M, m);
        }
    }

    return M;
}
// [REVIEW]
static bitboard get_knight_map(square from, Whose whose, position_t P) {
    return KNIGHT_ATTACKS[from] & ~P->whose[whose];
}
// [REVIEW]
static movelist *generate_knight_moves(movelist *M, square from, position_t P) {
    square to;
    bitboard knight_map = get_knight_map(from, OURS, P);

    bitboard captures = knight_map & P->whose[THEIRS];
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { KNIGHT, from, to, M_FLAG_CAPTURE };
        movelist_append(M, m);
    }

    bitboard quiet_moves = knight_map & ~P->whose[THEIRS];
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { KNIGHT, from, to, M_FLAG_QUIET };
        movelist_append(M, m);
    }

    return M;
}
// [REVIEW]
static bitboard get_diagonal_sliding_map(square from, Whose whose, position_t P) {
    bitboard diagonal_map, blockers, block;
    bool is_forward;
    diagonal_map = BITBOARD_EMPTY;

    for (int dir = NORTH_EAST; dir <= NORTH_WEST; dir++) {
        blockers = RAYS[dir][from] & (P->whose[OURS] | P->whose[THEIRS]);
        is_forward = dir == NORTH_EAST || dir == NORTH_WEST;
        block = is_forward ? bitboard_bsf(blockers) : bitboard_bsr(blockers);
        diagonal_map |= RAYS[dir][from] & (block < 64 ? ~RAYS[dir][block] : BITBOARD_FULL);
    }
    return diagonal_map & ~P->whose[whose];
}
// [REVIEW]
static movelist *generate_bishop_moves(movelist *M, square from, position_t P)  {
    square to;
    bitboard bishop_map = get_diagonal_sliding_map(from, OURS, P);

    bitboard captures = bishop_map & P->whose[THEIRS];
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { BISHOP, from, to, M_FLAG_CAPTURE };
        movelist_append(M, m);
    }

    bitboard quiet_moves = bishop_map & ~P->whose[THEIRS];
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { BISHOP, from, to, M_FLAG_QUIET };
        movelist_append(M, m);
    }

    return M;
}
// [REVIEW]
static bitboard get_straight_sliding_map(square from, Whose whose, position_t P) {
    bitboard straight_map, blockers, block;
    bool is_forward;
    straight_map = BITBOARD_EMPTY;

    for (int dir = NORTH; dir <= WEST; dir++) {
        blockers = RAYS[dir][from] & (P->whose[OURS] | P->whose[THEIRS]);
        is_forward = dir == NORTH || dir == EAST;
        block = is_forward ? bitboard_bsf(blockers) : bitboard_bsr(blockers);
        straight_map |= RAYS[dir][from] & (block < 64 ? ~RAYS[dir][block] : BITBOARD_FULL);
    }

    return straight_map & ~P->whose[whose];
}

// [REVIEW]
static movelist *generate_rook_moves(movelist *M, square from, position_t P) {
    square to;
    bitboard rook_map = get_straight_sliding_map(from, OURS, P);

    bitboard captures = rook_map & P->whose[THEIRS];
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { ROOK, from, to, M_FLAG_CAPTURE };
        movelist_append(M, m);
    }

    bitboard quiet_moves = rook_map & ~P->whose[THEIRS];
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { BISHOP, from, to, M_FLAG_QUIET };
        movelist_append(M, m);
    }

    return M;
}
// [REVIEW]
static movelist *generate_queen_moves(movelist *M, square from, position_t P) {
    square to;
    bitboard queen_map = get_diagonal_sliding_map(from, OURS, P) | 
                         get_straight_sliding_map(from, OURS, P);

    bitboard captures = queen_map & P->whose[THEIRS];
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { QUEEN, from, to, M_FLAG_CAPTURE };
        movelist_append(M, m);
    }

    bitboard quiet_moves = queen_map & ~P->whose[THEIRS];
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { QUEEN, from, to, M_FLAG_QUIET };
        movelist_append(M, m);
    }

    return M;
}
// [REVIEW]
static bitboard get_king_map(square from, Whose whose, position_t P) {
    return KING_ATTACKS[from] & ~P->whose[OURS];
}
// [REVIEW]
static movelist *generate_king_moves(movelist *M, square from, position_t P) {
    square to;
    bitboard all = P->whose[OURS] | P->whose[THEIRS];
    bitboard king_map = get_king_map(from, OURS, P);
    
    bitboard captures = king_map & P->whose[THEIRS];
    while ((to = bitboard_iter_first(&captures)) != INVALID_SQUARE) {
        move m = { KING, from, to, M_FLAG_CAPTURE };
        movelist_append(M, m);
    }

    bitboard quiet_moves = king_map & ~P->whose[THEIRS];
    while ((to = bitboard_iter_first(&quiet_moves)) != INVALID_SQUARE) {
        move m = { KING, from, to, M_FLAG_QUIET };
        movelist_append(M, m);
    }

    if (position_get_castling(P, OURS, KINGSIDE) && 
        bitboard_is_empty(CASTLING_MASK[KINGSIDE] & all)) {
        move m = { KING, 0, 0, M_FLAG_CASTLING[KINGSIDE] };
        movelist_append(M, m);
    }

    if (position_get_castling(P, OURS, QUEENSIDE) && 
        bitboard_is_empty(CASTLING_MASK[QUEENSIDE] & all)) {
        move m = { KING, 0, 0, M_FLAG_CASTLING[QUEENSIDE] };
        movelist_append(M, m);
    }

    return M;
}
// [REVIEW]
movelist *generate_moves(movelist *M, position_t P) {
    square from;
    bitboard our_pawns = P->whose[OURS] & P->pieces[PAWN];
    while ((from = bitboard_iter_first(&our_pawns)) != INVALID_SQUARE) {
        generate_pawn_moves(M, from, P);
    }

    bitboard our_knights = P->whose[OURS] & P->pieces[KNIGHT];
    while ((from = bitboard_iter_first(&our_knights)) != INVALID_SQUARE) {
        generate_knight_moves(M, from, P);
    }

    bitboard our_bishops = P->whose[OURS] & P->pieces[BISHOP];
    while ((from = bitboard_iter_first(&our_bishops)) != INVALID_SQUARE) {
        generate_bishop_moves(M, from, P);
    }

    bitboard our_rooks = P->whose[OURS] & P->pieces[ROOK];
    while ((from = bitboard_iter_first(&our_rooks)) != INVALID_SQUARE) {
        generate_rook_moves(M, from, P);
    }

    bitboard our_queens = P->whose[OURS] & P->pieces[QUEEN];
    while ((from = bitboard_iter_first(&our_queens)) != INVALID_SQUARE) {
        generate_queen_moves(M, from, P);
    }
    
    from = P->king[OURS];
    generate_king_moves(M, from, P);

    return M;
}