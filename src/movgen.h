/*
  This file is part of Gargantua, a UCI chess engine with NNUE evaluation
  derived from Chess0, and inspired by Code Monkey King's bbc-1.4.
     
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

#ifndef MOVGEN_H
#define MOVGEN_H

#include <iostream>
#include <sstream>
#include <cassert>

#include "bitboard.h"
#include "position.h"



// MoveList_t is a structure holding a list of moves (up to 256, which is
// enough for any legal chess position), and a pointer to the last element,
// which can also be used as a counter of elements in the move list.
typedef struct {
    int moves[256];
    int count;
} MoveList_t;



// Move encoding:
//
// Moves are encoded using 24 bits, where the following schema is followed:
/*
          binary move bits                               hexadecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/

// Encode move macro
#define encodeMove(fromSq, toSq, piece, promo, capture, double, ep, castling) \
    (fromSq) |          \
    (toSq << 6) |     \
    (piece << 12) |     \
    (promo << 16) |  \
    (capture << 20) |   \
    (double << 21) |    \
    (ep << 22) | \
    (castling << 23)    \


// Extract source square
#define getMoveSource(move) (move & 0x3f)


// Extract target square
#define getMoveTarget(move) ((move & 0xfc0) >> 6)


// Extract piece
#define getMovePiece(move) ((move & 0xf000) >> 12)


// Extract promoted piece
#define getPromo(move) ((move & 0xf0000) >> 16)


// Extract capture flag
#define getMoveCapture(move) (move & 0x100000)


// Extract double pawn push flag
#define getDoublePush(move) (move & 0x200000)


// Extract enpassant flag
#define getEp(move) (move & 0x400000)


// Extract castling flag
#define getCastle(move) (move & 0x800000)



// Table for bookkeeping castling rights when the King or Rook(s)
// move.
/*
                           castling   move     in      in
                              right update     binary  decimal

 king & rooks didn't move:     1111 & 1111  =  1111    15

        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13
     
         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7

*/

// Castling rights update constants
static constexpr int CastlingRights[64] =
{
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};



// Functionality to generate and manipulate chess moves.
void generateMoves(MoveList_t &);
void generate_moves(MoveList_t &);
void printMoveList(MoveList_t &);



// prettyMove
//
// Generate a string with the move in UCI notation.
static inline std::string prettyMove(int move)
{
    std::stringstream ss;

    ss << SquareToCoordinates[getMoveSource(move)]
       << SquareToCoordinates[getMoveTarget(move)]
       << PromoPieces[getPromo(move)];

    return ss.str();
}



// printMove
//
// Print a move in UCI notation.
static inline void printMove(int move)
{
    std::cout << prettyMove(move);
}



// addMove
//
// Add a move to a move list.
static inline void addMove(MoveList_t &MoveList, int move)
{
    // strore move
    MoveList.moves[MoveList.count] = move;

    
    // increment move count
    MoveList.count++;
}



// isSquareAttacked
//
// True if the given square is attacked by any piece an opponent's piece.
static inline bool isSquareAttacked(int square, int side)
{
    // square attacked by White or Black pawns
    if ((side == White) && (PawnAttacks[Black][square] & bitboards[P]))
        return true;

    if ((side == Black) && (PawnAttacks[White][square] & bitboards[p]))
        return true;
   

    // square attacked by Knights
    if (KnightAttacks[square] & ((side == White) ? bitboards[N] : bitboards[n]))
        return true;

    
    // square attacked by Bishops
    if (getBishopAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[B] : bitboards[b]))
        return true;


    // square attacked by Rooks
    if (getRookAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[R] : bitboards[r]))
        return true;


    // square attacked by Queens
    if (getQueenAttacks(square, occupancies[Both]) & ((side == White) ? bitboards[Q] : bitboards[q]))
        return true;

    
    // square attacked by Kings
    if (KingAttacks[square] & ((side == White) ? bitboards[K] : bitboards[k]))
        return true;


    // by default return false
    return false;
}



// saveBoard
//
// Implemented as a macro, it's job is to preserve the current board state
// in temporary variables.
#define saveBoard()                                                       \
    Bitboard bitboards_copy[12], occupancies_copy[3];                          \
    int side_copy, enpassant_copy, castle_copy;                           \
    memcpy(bitboards_copy, bitboards, sizeof(bitboards));                 \
    memcpy(occupancies_copy, occupancies, sizeof(occupancies));            \
    side_copy = sideToMove, enpassant_copy = epsq, castle_copy = castle;  \



// takeBack
//
// Implemented as a macro, it's job is to restore the previous board state
// from temporary variables. Because this is a macro, it needs to be used
// in conjunction with saveBoard() within the same scope.
#define takeBack()                                                        \
    memcpy(bitboards, bitboards_copy, sizeof(bitboards));                 \
    memcpy(occupancies, occupancies_copy, sizeof(occupancies));           \
    sideToMove = side_copy, epsq = enpassant_copy, castle = castle_copy;  \



// Different move types for make/unmake move. This is a way to later 
// ask the engine to online reproduce captures for specific searches
// or functionalities. However AllMoves will be the standard flag
// when calling makeMove().
enum MoveType { AllMoves, CaptureMoves };



// makeMove
//
// Make move (thus alter the position) on the chess board.
//
// Note: remember to save the board status (saveBoard) before calling
//       makeMove(), if you then want to be able to use takeBack().
static inline int makeMove(int move, int flag)
{
    assert(st != NULL);

    std::cout << "Enter makeMove()" << std::endl;
    // Quiet moves:
    if (flag == AllMoves)
    {
        // parse move elements
        int fromSq   = getMoveSource(move);
        int toSq     = getMoveTarget(move);
        int piece    = getMovePiece(move);
        int promo    = getPromo(move);
        int capture  = getMoveCapture(move);
        int dpush    = getDoublePush(move);
        int ep       = getEp(move);
        int castling = getCastle(move);


        // configure opponent's color
        int Them     = White;
        if (sideToMove == White)
            Them = Black;
       

        // move the piece from source to target
        popBit(bitboards[piece], fromSq);
        setBit(bitboards[piece], toSq);


        // update occupancies for the piece being moved
        popBit(occupancies[sideToMove], fromSq);
        setBit(occupancies[sideToMove], toSq);
                    

        // after the moving piece, also handle the captured piece, if any
        if (capture)
        {
            // reset fifty move rule counter
            //fifty = 0;

            
            // pick up bitboard piece index ranges depending on side
            int start_piece, end_piece;
           

            // configure side to move (in order to reduce the piece)
            if (sideToMove == White)
            {
                start_piece = p;
                end_piece = k;
            }
            else
            {
                start_piece = P;
                end_piece = K;
            }

            
            // if there's a piece on the target, remove it from the bitboard
            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
            {
                if (getBit(bitboards[bb_piece], toSq))
                {
                    popBit(bitboards[bb_piece], toSq);

                    // update occupancies for the piece just removed
                    popBit(occupancies[Them], toSq);

                    // StateInfo
                    st->capturedPiece = bb_piece;

                    // remove the piece from hash key
                    //hash_key ^= piece_keys[bb_piece][target_square];
                    break;
                }
            }
        }


        // handle pawn promotions
        if (promo)
        {
            // white to move
            if (sideToMove == White)
            {
                // erase the pawn from the target square
                popBit(bitboards[P], toSq);

                // remove pawn from hash key
                //hash_key ^= piece_keys[P][toSq];
            }

            
            // black to move
            else
            {
                // erase the pawn from the target square
                popBit(bitboards[p], toSq);
                
                // remove pawn from hash key
                //hash_key ^= piece_keys[p][toSq];
            }

            
            // set promoted piece on the chess board
            setBit(bitboards[promo], toSq);

            
            // add promoted piece into the hash key
            //hash_key ^= piece_keys[promo][toSq];
        }


        // handle enpassant captures
        if (ep)
        {
            // erase the pawn depending on side to move
            (sideToMove == White) ? popBit(bitboards[p], toSq + 8) :
                                    popBit(bitboards[P], toSq - 8);
                     

            // white to move
            if (sideToMove == White)
            {
                // remove captured pawn
                popBit(bitboards[p], toSq + 8);

                // update occupancies
                popBit(occupancies[Black], toSq + 8);
                
                // remove pawn from hash key
                //hash_key ^= piece_keys[p][toSq + 8];
            }
           

            // black to move
            else
            {
                // remove captured pawn
                popBit(bitboards[P], toSq - 8);

                // update occupancies
                popBit(occupancies[White], toSq - 8);
                
                // remove pawn from hash key
                //hash_key ^= piece_keys[P][toSq - 8];
            }
        }


        // hash enpassant if available (remove enpassant square from hash key)
        //if (ep != NoSq) hash_key ^= enpassant_keys[ep];
       

        // reset enpassant square
        epsq = NoSq;


        // handle double pawn pushes
        if (dpush)
        {
            // white to move
            if (sideToMove == White)
            {
                // set enpassant square
                epsq = toSq + 8;
                
                // hash enpassant
                //hash_key ^= enpassant_keys[toSq + 8];
            }
            
            // black to move
            else
            {
                // set enpassant square
                epsq = toSq - 8;
                
                // hash enpassant
                //hash_key ^= enpassant_keys[target_square - 8];
            }
        }


        // handle castling moves
        if (castling)
        {
            // four different possibilities: white and black, 0-0 and 0-0-0
            switch (toSq)
            {
                // white castles king side (0-0)
                case (g1):
                    // move rook from h1
                    popBit(bitboards[R], h1);
                    setBit(bitboards[R], f1);

                    // update occupancies
                    popBit(occupancies[White], h1);
                    setBit(occupancies[White], f1);
                    
                    // hash rook
                    //hash_key ^= piece_keys[R][h1];  // remove rook from h1 from hash key
                    //hash_key ^= piece_keys[R][f1];  // put rook on f1 into a hash key
                    break;
               

                // white castles queen side (0-0-0)
                case (c1):
                    // move rook from a1
                    popBit(bitboards[R], a1);
                    setBit(bitboards[R], d1);

                    // update occupancies
                    popBit(occupancies[White], a1);
                    setBit(occupancies[White], d1);
                    
                    // hash rook
                    //hash_key ^= piece_keys[R][a1];  // remove rook from a1 from hash key
                    //hash_key ^= piece_keys[R][d1];  // put rook on d1 into a hash key
                    break;
               

                // black castles king side (0-0)
                case (g8):
                    // move rook from h8
                    popBit(bitboards[r], h8);
                    setBit(bitboards[r], f8);

                    // update occupancies
                    popBit(occupancies[Black], h8);
                    setBit(occupancies[Black], f8);
                    
                    // hash rook
                    //hash_key ^= piece_keys[r][h8];  // remove rook from h8 from hash key
                    //hash_key ^= piece_keys[r][f8];  // put rook on f8 into a hash key
                    break;
               

                // black castles queen side (0-0-0)
                case (c8):
                    // move rook from a8
                    popBit(bitboards[r], a8);
                    setBit(bitboards[r], d8);

                    // update occupancies
                    popBit(occupancies[Black], a8);
                    setBit(occupancies[Black], d8);
                    
                    // hash rook
                    //hash_key ^= piece_keys[r][a8];  // remove rook from a8 from hash key
                    //hash_key ^= piece_keys[r][d8];  // put rook on d8 into a hash key
                    break;
            }
        }


        // hash castling
        //hash_key ^= castle_keys[castle];

        
        // update castling rights
        castle &= CastlingRights[fromSq];
        castle &= CastlingRights[toSq];


        // update StateInfo
        st->castle = castle;
        st->epsq = epsq;


        // update all occupancies
        occupancies[Both] = occupancies[White] | occupancies[Black];


        // change side to move
        sideToMove ^= 1;
        //hash_key ^= side_key;

        std::cout << "Leave makeMove()" << std::endl;

        
        // check if move is legal (return 0 for illegal move, 1 for legal)
        if (isSquareAttacked((sideToMove == White) ? ls1b(bitboards[k]) : ls1b(bitboards[K]), sideToMove))
            return 0;
        else
            return 1;
    }

    
    // Capture moves
    else
    {
        std::cout << "Leave makeMove()" << std::endl;
        // make sure move is the capture
        if (getMoveCapture(move))
            makeMove(move, AllMoves);
       

        // otherwise the move is not a capture, so don't make it
        else
            return 0;
    }


    // return 0 as 'illegal move' if nothing happens
    return 0;
}



// undoMove
//
// Unmakes a move on the chess board. When it returns, the position should
// be restored to exactly the same state as before the move was made.
static inline void undoMove(int m)
{
    assert(st != NULL);

    std::cout << "Enter undoMove()" << std::endl;
    sideToMove ^= 1;

    int fromSq = getMoveSource(m);
    int toSq   = getMoveTarget(m);
    int piece  = getMovePiece(m);
    int promo  = getPromo(m);


    // undo promotions
    if (getPromo(m))
    {
        if (sideToMove == White)
        {
            switch (promo)
            {
                case N: popBit(bitboards[N], toSq);
                        break;

                case B: popBit(bitboards[B], toSq);
                        break;

                case R: popBit(bitboards[R], toSq);
                        break;

                case Q: popBit(bitboards[Q], toSq);
                        break;

                default: break;
            }

            // restore pawn to original square, and empty target square
            popBit(occupancies[White], toSq);
            setBit(bitboards[P], fromSq);
            setBit(occupancies[White], fromSq);
        }

        else
        {
            switch (promo)
            {
                case N: popBit(bitboards[n], toSq);
                        break;

                case B: popBit(bitboards[b], toSq);
                        break;

                case R: popBit(bitboards[r], toSq);
                        break;

                case Q: popBit(bitboards[q], toSq);
                        break;

                default: break;
            }

            // restore pawn to original square, and empty target square
            popBit(occupancies[Black], toSq);
            setBit(bitboards[p], fromSq);
            setBit(occupancies[Black], fromSq);
        }
    }


    // undo castling
    else if (getCastle(m))
    {
        // White castling
        if (fromSq == e1)
        {
            // undo moving the King
            popBit(bitboards[K], toSq);
            popBit(occupancies[White], toSq);
            setBit(bitboards[K], e1);
            setBit(occupancies[White], e1);

            // undo moving the Rook
            if (toSq == g1)
            {
                popBit(bitboards[R], f1);
                popBit(occupancies[White], f1);
                setBit(bitboards[R], h1);
                setBit(occupancies[White], h1);
            }
            else
            {
                popBit(bitboards[R], d1);
                popBit(occupancies[White], d1);
                setBit(bitboards[R], a1);
                setBit(occupancies[White], a1);
            }
        }

        // Black castling
        else
        {
            // undo moving the King
            popBit(bitboards[K], toSq);
            popBit(occupancies[White], toSq);
            setBit(bitboards[K], e8);
            setBit(occupancies[White], e8);

            // undo moving the Rook
            if (toSq == g8)
            {
                popBit(bitboards[R], f8);
                popBit(occupancies[Black], f8);
                setBit(bitboards[R], h8);
                setBit(occupancies[Black], h8);
            }
            else
            {
                popBit(bitboards[R], d8);
                popBit(occupancies[Black], d8);
                setBit(bitboards[R], a8);
                setBit(occupancies[Black], a8);
            }
        }
    }

   
    // undo any other moves (including captures)
    else
    {
        switch (piece)
        {
            case P: popBit(bitboards[P], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[P], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case N: popBit(bitboards[N], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[N], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case B: popBit(bitboards[B], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[B], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case R: popBit(bitboards[R], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[R], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case Q: popBit(bitboards[Q], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[Q], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case K: popBit(bitboards[K], toSq);
                    popBit(occupancies[White], toSq);
                    setBit(bitboards[K], fromSq);
                    setBit(occupancies[White], fromSq);
                    break;

            case p: popBit(bitboards[p], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[p], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;

            case n: popBit(bitboards[n], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[n], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;

            case b: popBit(bitboards[b], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[b], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;

            case r: popBit(bitboards[r], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[r], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;

            case q: popBit(bitboards[q], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[q], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;

            case k: popBit(bitboards[k], toSq);
                    popBit(occupancies[Black], toSq);
                    setBit(bitboards[k], fromSq);
                    setBit(occupancies[Black], fromSq);
                    break;
        }


        // restore the captured piece, if any
        if ((st->capturedPiece >= P) && (st->capturedPiece <= k))
        {
            int capsq = toSq;

            if (getEp(m))
            {
                if (sideToMove == White)
                    capsq = toSq + 8;
                else
                    capsq = toSq - 8;
            }

            setBit(bitboards[st->capturedPiece], capsq);
        }
    }

    occupancies[Both] = occupancies[White] | occupancies[Black];


    // Finally point our state pointer back to the previous state
    st = st->previous;
    castle = st->castle;

    std::cout << "Leave undoMove()" << std::endl;
}



#endif  //  MOVGEN_H
