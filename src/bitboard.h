/*
  This file is part of Gargantua, a UCI chess engine derived from Chess0 which
  uses NNUE for evaluation. This engine has been inspired by BBC-1.4 (by Code
  Monkey King), Blunder (by Christian Dean) and Stockfish.
     
  Copyright (C) 2022 Claudio M. Camacho
 
  Gargantua is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  Gargantua is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITBOARD_H
#define BITBOARD_H



#include "definitions.h"



// board square masks, including different files, ranks and square groups
constexpr Bitboard AllSquares    = ~Bitboard(0);
constexpr Bitboard DarkSquares   = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard LightSquares  = ~(0xAA55AA55AA55AA55ULL);

constexpr Bitboard FileA_Mask = 0x0101010101010101ULL;
constexpr Bitboard FileB_Mask = FileA_Mask << 1;
constexpr Bitboard FileC_Mask = FileA_Mask << 2;
constexpr Bitboard FileD_Mask = FileA_Mask << 3;
constexpr Bitboard FileE_Mask = FileA_Mask << 4;
constexpr Bitboard FileF_Mask = FileA_Mask << 5;
constexpr Bitboard FileG_Mask = FileA_Mask << 6;
constexpr Bitboard FileH_Mask = FileA_Mask << 7;

constexpr Bitboard Rank1_Mask = 0xFF;
constexpr Bitboard Rank2_Mask = Rank1_Mask << (8 * 1);
constexpr Bitboard Rank3_Mask = Rank1_Mask << (8 * 2);
constexpr Bitboard Rank4_Mask = Rank1_Mask << (8 * 3);
constexpr Bitboard Rank5_Mask = Rank1_Mask << (8 * 4);
constexpr Bitboard Rank6_Mask = Rank1_Mask << (8 * 5);
constexpr Bitboard Rank7_Mask = Rank1_Mask << (8 * 6);
constexpr Bitboard Rank8_Mask = Rank1_Mask << (8 * 7);

constexpr Bitboard NotFileA_Mask  = 18374403900871474942ULL;
constexpr Bitboard NotFileH_Mask  = 9187201950435737471ULL;
constexpr Bitboard NotFileHG_Mask = 4557430888798830399ULL;
constexpr Bitboard NotFileAB_Mask = 18229723555195321596ULL;


// bishop relevant occupancy bit count for every square on board
constexpr int BishopRelevantBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

// rook relevant occupancy bit count for every square on board
constexpr int RookRelevantBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};



// rook magic numbers
constexpr Bitboard RookMagicNumbers[64] =
{
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};


// bishop magic numbers
constexpr Bitboard BishopMagicNumbers[64] =
{
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};



// Functions for initializing, printing and manipulating Bitboard data
// structures and information:
void printBitboard(Bitboard);
std::string pretty(Bitboard);
Bitboard maskBishopAttacks(int);
Bitboard setOccupancy(int, int, Bitboard);
void initLeaperAttacks();
uint64_t genMagicNumber();



// rng32
//
// Generate a 32-bit pseudo-random number.
static inline uint32_t rng32()
{
    // get current state
    uint32_t number = rng32_state;
   

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
   

    // update random number state
    rng32_state = number;
   

    // return random number
    return number;
}



// rng64
//
// Generate a 64-bit pseudo-random number.
static inline uint64_t rng64()
{
    // define 4 random numbers
    uint64_t n1, n2, n3, n4;
   

    // init random numbers slicing 16 bits from MS1B side
    n1 = (uint64_t)(rng32() & 0xFFFF);
    n2 = (uint64_t)(rng32() & 0xFFFF);
    n3 = (uint64_t)(rng32() & 0xFFFF);
    n4 = (uint64_t)(rng32() & 0xFFFF);
   

    // return random number
    return (uint64_t) (n1 | (n2 << 16) | (n3 << 32) | (n4 << 48));
}



// Bitboard and bitwise operations:
//
// They can be defined both as macros (using #define) or as static inline
// functions. The difference is that, with macros, they are treated as pure
// test replacements by the preprocessor, so no typechecking is involved and
// it's more difficult to debug.
//
// However, static inline functions will only be *as fast* as macros when using
// optimizations (i.e., -Ofast or -O3), because the compiler will replace the
// function calls with inline code. So, remember to use optimizations!
//
// Below, there is a list of all the functions, plus how would they look like as 
// macros, in the comments.



// getBit
//
// #define getBit(b, pos) (b & (1ULL << pos))
static inline int getBit(Bitboard b, int pos)
{
    return ((b >> pos) & 1ULL);
}



// setBit
//
// #define setBit(b, pos) (b |= (1ULL << pos))
static inline void setBit(Bitboard &b, int pos)
{
    b |= (1ULL << pos);
}



// clearBit
//
// #define clearBit(b, pos) (b &= ~(1ULL << pos))
static inline void clearBit(Bitboard &b, int pos)
{
    b &= ~(1ULL << pos);
}



// toggleBit
//
// #define toggleBit(b, pos) (b ^= (1ULL << pos))
static inline void toggleBit(Bitboard &b, int pos)
{
    b ^= (1ULL << pos);
}



// countBits
//
// Count the number of bits within a Bitboard using the MIT HAKMEM algorithm.
//
// @see http://graphics.stanford.edu/~seander/bithacks.html
static inline int countBits(Bitboard bb)
{
    static const Bitboard  M1 = 0x5555555555555555;  // 1 zero,  1 one ...
    static const Bitboard  M2 = 0x3333333333333333;  // 2 zeros,  2 ones ...
    static const Bitboard  M4 = 0x0f0f0f0f0f0f0f0f;  // 4 zeros,  4 ones ...
    static const Bitboard  M8 = 0x00ff00ff00ff00ff;  // 8 zeros,  8 ones ...
    static const Bitboard M16 = 0x0000ffff0000ffff;  // 16 zeros, 16 ones ...
    static const Bitboard M32 = 0x00000000ffffffff;  // 32 zeros, 32 ones


    bb = (bb & M1 ) + ((bb >>  1) & M1 );   //put count of each  2 bits into those  2 bits
    bb = (bb & M2 ) + ((bb >>  2) & M2 );   //put count of each  4 bits into those  4 bits
    bb = (bb & M4 ) + ((bb >>  4) & M4 );   //put count of each  8 bits into those  8 bits
    bb = (bb & M8 ) + ((bb >>  8) & M8 );   //put count of each 16 bits into those 16 bits
    bb = (bb & M16) + ((bb >> 16) & M16);   //put count of each 32 bits into those 32 bits
    bb = (bb & M32) + ((bb >> 32) & M32);   //put count of each 64 bits into those 64 bits


    return (int)bb;
}



// ls1b
//
// Find the first bit on a Bitboard using the De Bruijn Multiplication
// @see http://chessprogramming.wikispaces.com/BitScan
//
// Note: don't use this if bb = 0
static inline unsigned int ls1b(Bitboard bb)
{
    static constexpr int INDEX64[64] = {
        63,  0, 58,  1, 59, 47, 53,  2,
        60, 39, 48, 27, 54, 33, 42,  3,
        61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22,  4,
        62, 57, 46, 52, 38, 26, 32, 41,
        50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16,  9, 12,
        44, 24, 15,  8, 23,  7,  6,  5  };

    static constexpr Bitboard DEBRUIJN64 = Bitboard(0x07EDD5E59A4E28C2);

    return INDEX64[((bb & -bb) * DEBRUIJN64) >> 58];  
}



#endif  // BITBOARD_H
