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

// This file aims to generate legal moves for the sliding
// pieces that is, rooks, bishops & queens, assuming a
// completely empty board.This file is used to help the file 
// "K_bit_magics.cpp".

#include <iostream>
#include "Slider_Movers.hpp"

// All directions are from white's perspective
BB_Value Gen_Rook_Moves(Square sq, bool blocker_mask)
{
  assertm(DIRECTION_TBLS_INITIALISED, 
    "Directional tables must be initialised first!");

  BB_Value Moves = Remove_Bit((FILES[sq] | RANKS[sq]), sq);
  BB_Value edges;
  
  if (blocker_mask)
  {
    edges  = Remove_Bit((FILES[file_A] | FILES[file_H]), FILES[sq]);
    edges |= Remove_Bit((RANKS[rank_1] | RANKS[rank_8]), RANKS[sq]);
  }

  else edges = 0;

  return Remove_Bit(Moves, edges);
}

BB_Value Gen_Bishop_Moves(Square sq, bool blocker_mask)
{
  assertm(DIRECTION_TBLS_INITIALISED, 
    "Directional tables must be initialised first!");

  BB_Value Moves = Remove_Bit(DIAGS[sq], sq);
  BB_Value edges;

  if (blocker_mask)
  {
    edges  = Remove_Bit((FILES[file_A] | FILES[file_H]), FILES[sq]);
    edges |= Remove_Bit((RANKS[rank_1] | FILES[rank_8]), RANKS[sq]);
  }

  else edges = 0;

  return Remove_Bit(Moves, edges);
}

BB_Value Gen_Queen_Moves(Square sq, bool blocker_mask)
{
  assertm(DIRECTION_TBLS_INITIALISED, 
    "Directional tables must be initialised first!");

  BB_Value Moves = Gen_Rook_Moves(sq, blocker_mask) | 
	           Gen_Bishop_Moves(sq, blocker_mask);
  return Moves;
}
