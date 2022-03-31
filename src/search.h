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

#ifndef SEARCH_H
#define SEARCH_H

#include <cassert>

#include "movgen.h"



using namespace std;



// Default settings and configuration for the search, as well as 
// tuning parameters for search extensions and reductions:
#define DEFAULT_SEARCH_DEPTH         256
#define DEFAULT_SEARCH_MOVETIME_MS  5000
#define LMR_FULLDEPTH_MOVES            4
#define LMR_REDUCTION_LIMIT            3
#define ASPIRATION_WINDOW_SIZE        60



// Search definitions, including alpha-beta bounds, mating scores, etc.
#define DRAWSCORE           0
#define MATEVALUE       49000
#define MATESCORE       48000
#define VALUE_INFINITE  50000



// Maximum depth at which we try to search
#define MAXPLY       256



// Score assigned to non-capture promotions. This is used for
// sorting moves based on their likeliness to be good.
//
// @see scoreMove() and sortMoves()
#define MOVESCORE_PROMO_QUIET   10000



// 'nodes' is a global variable holding the number of nodes analyzed
// or searched. It is used by negamax() but also other performance test
// functions such as perft().
extern uint64_t nodes;



// Limits_t is a structure that holds the configuration of the search.
// This includes search depth, time to search, etc.
//
// The engine uses the global variable "limits" to set, edit and reset the
// search configuration throught the entire lifecycle.
typedef struct
{
    int wtime;
    int btime;
    int winc;
    int binc;
    int npmsec;
    int movetime;
    int movestogo;
    int depth;
    int mate;
    int perft;
    bool infinite;
    int nodes;
} Limits_t;

extern Limits_t Limits;



// killers [id][ply] 
//
// Killers is a table where the two best (quiet) moves are
// systematically stored for later searches. This is based on the
// fact that a move producing a beta cut-off must be a good one.
// beta cut-offs, where a move killer moves [id][ply]
//
// Note: storing exactly 2 killer moves is best for efficiency/performance.
//
// @see https://www.chessprogramming.org/Killer_Heuristic
extern int killers[2][MAXPLY];



// history [piece][square]
//
// History is a table where to store moves that have produced an improvement in
// the score of previous searches. In other words, they have raised alpha.
//
// @see https://www.chessprogramming.org/History_Heuristic
extern int history[12][64];




/*
      ================================
            Triangular PV table
      --------------------------------
        PV line: e2e4 e7e5 g1f3 b8c6
      ================================

           0    1    2    3    4    5
      
      0    m1   m2   m3   m4   m5   m6
      
      1    0    m2   m3   m4   m5   m6 
      
      2    0    0    m3   m4   m5   m6
      
      3    0    0    0    m4   m5   m6
       
      4    0    0    0    0    m5   m6
      
      5    0    0    0    0    0    m6
*/

// PV length [ply]
//
// An array of Principal Variations indexed by ply (distance to root). This is
// employed to collect the principal variation of best moves inside the
// alpha-beta or principal variation search, with the best score moves
// propagated up to the root.
//
// @see https://www.chessprogramming.org/Triangular_PV-Table
extern int pv_length[MAXPLY];



// PV table [ply][ply]
extern int pv_table[MAXPLY][MAXPLY];



// follow PV & score PV move
extern bool followPV, scorePV;



// flag to control whether we allow null move pruning or not
extern bool allowNull;



// Most Valuable Victim / Less Valuable Attacker (MVV/LVA) lookup table
/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600
*/

// MVV LVA [attacker][victim]
//
// A simple heuristic to generate or sort capture moves in a reasonable order.
// Inside a so called find-victim cycle, one first look up the potential
// victim of all attacked opponent pieces, in the order of the most valuable
// first, thus queen, rook, bishop, knight and pawn. After the most valuable
// victim is found, the find-aggressor cycle loops over the potential aggressors
// that may capture the victim in inverse order, from pawn, knight, bishop, rook,
// queen to king. 
//
// @see https://www.chessprogramming.org/MVV-LVA
static constexpr int mvv_lva[12][12] =
{
 	{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600},

	{105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600}
};



// Functionality to search a position or perform an operation on the
// nodes of a given position.
void dperft(int);
void search();
int  qsearch(int, int);
void initSearch();
void resetLimits();
void sortMoves(MoveList_t &);
void printMoveScores(MoveList_t &);



// perft
//
// Verify move generation. All the leaf nodes up to the given depth are
// generated and counted.
// 
// @see https://www.chessprogramming.org/Perft
static inline void perft(int depth)
{
    // reliability checks
    assert(depth >= 0);


    // escape at leaf nodes and increment node count
    if (depth == 0)
    {
        nodes++;
        return;
    }

    
    // create move list instance
    MoveList_t MoveList;

    
    // generate moves
    generateMoves(MoveList);

    
    // loop over generated moves
    for (int move_count = 0; move_count < MoveList.count; move_count++)
    {   
        // preserve board state
        saveBoard();


        // make move and, if illegal, skip to the next move
        if (!makeMove(MoveList.moves[move_count]))
        {
            takeBack();
            continue;
        }


        // call perft driver recursively
        perft(depth - 1);

        
        // undo move
        takeBack();
    }
}



/*  =======================
         Move ordering
    =======================
    
    1. PV move
    2. Captures in MVV/LVA
    3. Promotions
    4. 1st killer move
    5. 2nd killer move
    6. History moves
    7. Unsorted moves
*/

// scoreMove
//
// Assign a score to a move.
static inline int scoreMove(int move)
{
    // if PV move scoring is allowed
    // if PV move and scoring allowed, assign it the highest score
    if (scorePV && (pv_table[0][ply] == move))
    {
        // disable score PV flag
        scorePV = false;
         
        // give PV move the highest score to search it first
        return 20000;
    }


    // score capture move
    else if (getMoveCapture(move))
    {
        // init target piece
        int target_piece = P;
        int toSq = getMoveTarget(move);

        
        // pick up bitboard piece index ranges depending on side
        int start_piece, end_piece;

        
        // pick up side to move
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

        
        // loop over the opponent's bitboards
        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++)
        {
            // if there's a piece on the target square
            if (getBit(bitboards[bb_piece], toSq))
            {
                // remove it from corresponding bitboard
                target_piece = bb_piece;
                break;
            }
        }
               

        // score move by MVV LVA lookup [source piece][target piece]
        return mvv_lva[getMovePiece(move)][target_piece] + 10000;
    }


    // quiet promotions are also scored
    else if (getPromo(move))
    {
        return MOVESCORE_PROMO_QUIET;
    }
   

    // score quiet move
    else
    {
        // score 1st killer move
        if (killers[0][ply] == move)
            return 9000;
        
        // score 2nd killer move
        else if (killers[1][ply] == move)
            return 8000;
        
        // score history move
        else
            return history[getMovePiece(move)][getMoveTarget(move)];
    }
   

    // by default, don't add a score to the move
    return 0;
}



// enablePV_scoring
//
// Allow scoring PV moves.
static inline void enablePV_scoring(MoveList_t &MoveList)
{
    // disable following PV
    followPV = false;

    
    // loop over the moves within a move list
    for (int count = 0; count < MoveList.count; count++)
    {
        // make sure we hit PV move
        if (pv_table[0][ply] == MoveList.moves[count])
        {
            // enable move scoring and follow PV again
            scorePV  = true;
            followPV = true;
        }
    }
}



#endif  //  SEARCH_H
