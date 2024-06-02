/*
    Evergreen: A didactic C++ chess engine 
    Copyright (C) 2024  Jonathan Kasongo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// This file aims to pre-compute possible moves for the static
// pieces that is, King, Pawn & Knight, assuming a completely
// empty board. Possible moves are indicated by 1's on a
// bitboard.

#include <iostream>
#include "Static_Movers.hpp"

BB_Value Gen_King_Moves(Square sq)
{
  BB_Value Moves = 0;

  Moves |= North(sq);
  Moves |= North_East(sq) & NOT_FILES[file_A];
  Moves |= East(sq)       & NOT_FILES[file_A];
  Moves |= South_East(sq) & NOT_FILES[file_A];
  Moves |= South(sq);
  Moves |= South_West(sq) & NOT_FILES[file_H];
  Moves |= West(sq)       & NOT_FILES[file_H];
  Moves |= North_West(sq) & NOT_FILES[file_H];

  return Moves;
}

BB_Value Gen_Knight_Moves(Square sq)
{
  BB_Value Moves = 0;
  
  Moves |= North( North_East(sq) ) & NOT_FILES[file_A];
  Moves |= East( North_East(sq) )  & (NOT_FILES[file_A] & NOT_FILES[b1]);
  Moves |= East( South_East(sq) )  & (NOT_FILES[file_A] & NOT_FILES[b1]);
  Moves |= South( South_East(sq) ) & NOT_FILES[file_A];
  Moves |= South( South_West(sq) ) & NOT_FILES[file_H];
  Moves |= West( South_West(sq) )  & (NOT_FILES[file_G] & NOT_FILES[file_H]);
  Moves |= West( North_West(sq) )  & (NOT_FILES[file_G] & NOT_FILES[file_H]);
  Moves |= North( North_West(sq) ) & NOT_FILES[file_H];

  return Moves;
}

BB_Value Gen_W_Pawn_Moves(Square sq)
{
  BB_Value Moves = 0;
  
  Moves |= North(sq);

  // Can make a double move if on the starting square
  if (sq >= h2 && sq <= a2) 
    Moves |= North( North(sq) );

  return Moves;
}

BB_Value Gen_B_Pawn_Moves(Square sq)
{
  BB_Value Moves = 0;
  
  Moves |= South(sq);

  // Can make a double move if on the starting square
  if (sq >= h7 && sq <= a7) 
    Moves |= South( South(sq) );

  return Moves;
}

BB_Value Gen_W_Pawn_Atks(Square sq)
{
  BB_Value Moves = 0;

  Moves |= North_East(sq) & NOT_FILES[file_A];
  Moves |= North_West(sq) & NOT_FILES[file_H];

  return Moves;
}

BB_Value Gen_B_Pawn_Atks(Square sq)
{
  BB_Value Moves = 0;

  Moves |= South_East(sq) & NOT_FILES[file_A];
  Moves |= South_West(sq) & NOT_FILES[file_H];

  return Moves;
}

BB_Value KING_MOVES[64];
BB_Value KNIGHT_MOVES[64];
BB_Value W_PAWN_MOVES[64];
BB_Value B_PAWN_MOVES[64];
BB_Value W_PAWN_ATKS[64];
BB_Value B_PAWN_ATKS[64];

void Init_Static_Piece_Tables()
{
  assertm(DIRECTION_TBLS_INITIALISED,
    "Directtional tables must be initialised first! - (Static pc initialisation)");

  STATIC_PC_TBLS_INITIALISED = true;

  for (Square sq = h1; sq <= a8; sq++)
  {
    KING_MOVES[sq]   = Gen_King_Moves(sq);
    KNIGHT_MOVES[sq] = Gen_Knight_Moves(sq);
    W_PAWN_MOVES[sq] = Gen_W_Pawn_Moves(sq);
    B_PAWN_MOVES[sq] = Gen_B_Pawn_Moves(sq);
    W_PAWN_ATKS[sq]  = Gen_W_Pawn_Atks(sq);
    B_PAWN_ATKS[sq]  = Gen_B_Pawn_Atks(sq);
  }
}
