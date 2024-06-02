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
// pieces that is, rooks, bishops & queens This file is used 
// to help the file "Tables.cpp".

#include <iostream>
#include "Slider_Movers.hpp"

// All directions are from white's perspective
BB_Value Gen_Rook_Moves_slow(Square sq, BB_Value Occupancy)
{
  BB_Value moves = 0; 
  Square Curr_sq;
  int dist;

  // If occupacy includes our square we assume that this 
  // is our own piece and we ignore it`
  if (Occupancy & Square_To_BB(sq))
    Occupancy = Remove_Bit(Occupancy, sq);

  // North
  Curr_sq = sq;
  dist    = Dist_To_North(sq);

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_North(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }
 
  // East 
  Curr_sq = sq;
  dist    = Dist_To_East(Curr_sq);

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_East(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  // South 
  Curr_sq = sq;
  dist    = Dist_To_South(sq);

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_South(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  // West
  Curr_sq = sq;
  dist    = Dist_To_West(sq);

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_West(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  return moves;
}

BB_Value Gen_Bishop_Moves_slow(Square sq, BB_Value Occupancy)
{
  BB_Value moves = 0; 
  Square Curr_sq;
  int dist;

  // North-east
  Curr_sq = sq;
  dist    = std::min(Dist_To_North(Curr_sq),
		     Dist_To_East(Curr_sq));
  
  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_North_East(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  // South-east
  Curr_sq = sq;
  dist    = std::min(Dist_To_South(Curr_sq),
		     Dist_To_East(Curr_sq));

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_South_East(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  // South-west
  Curr_sq = sq;
  dist    = std::min(Dist_To_South(Curr_sq),
		     Dist_To_West(Curr_sq));

  while ( (!(Square_To_BB(Curr_sq) & Occupancy)) &&
	    (dist-- > 0) )
  {
    Curr_sq = SQ_South_West(Curr_sq);
    moves   = Set_Bit(moves, Curr_sq);
  }

  return moves;
}

BB_Value Gen_Queen_Moves_slow(Square sq, BB_Value Occupancy)
{
  return Gen_Rook_Moves_slow(sq, Occupancy) |
	 Gen_Bishop_Moves_slow(sq, Occupancy);
}
