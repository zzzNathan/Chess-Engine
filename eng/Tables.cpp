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

#include "Tables.hpp"

std::unordered_map<BB_Value, BB_Value> ROOK_MOVES[64][4];
std::unordered_map<BB_Value, BB_Value> BISHOP_MOVES[64][4];

void Init_Rook_Tables()
{
  // For each sqaure...
  for (Square sq = h1; sq <= a8; sq++)
  {
    BB_Value LSB, MSB, Occ;
    int count;

    // Go through each of the rooks possible directions and 
    // precalculate legal move for all occupancies with ONLY
    // 1 bit set.

    // North
    Occ   = Remove_Bit(Make_File_North(sq), sq);
    count = Bit_Count(Occ);
    
    while (count--)
    {
      LSB = Get_LSB(Occ);

      if (Square_To_BB(sq) != LSB)
        ROOK_MOVES[sq][NORTH][LSB] = Gen_Rook_Moves_slow(sq, LSB);

      Pop_Bit(Occ, LSB);
    }

    // East
    Occ   = Remove_Bit(Make_Rank_East(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      MSB = Get_MSB(Occ);

      if (Square_To_BB(sq) != MSB)
        ROOK_MOVES[sq][EAST][MSB] = Gen_Rook_Moves_slow(sq, MSB);

      Pop_Bit(Occ, MSB);
    }

    // South
    Occ   = Remove_Bit(Make_File_South(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      MSB = Get_MSB(Occ);

      if (Square_To_BB(sq) != MSB)
        ROOK_MOVES[sq][SOUTH][MSB] = Gen_Rook_Moves_slow(sq, MSB);

      Pop_Bit(Occ, MSB);
    }

    // West
    Occ   = Remove_Bit(Make_Rank_West(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      LSB = Get_LSB(Occ);

      if (Square_To_BB(sq) != LSB)
        ROOK_MOVES[sq][WEST][LSB] = Gen_Rook_Moves_slow(sq, LSB);

      Pop_Bit(Occ, LSB);
    }
    
    for (auto Dir : {NORTH, EAST, SOUTH, WEST})
      ROOK_MOVES[sq][Dir][0] = Gen_Rook_Moves_slow(sq, 0);
  }
}

void Init_Bishop_Tables()
{
  // For each sqaure...
  for (Square sq = h1; sq <= a8; sq++)
  {
    BB_Value LSB, MSB, Occ;
    int count;

    // Go through each of the rooks possible directions and 
    // precalculate legal move for all occupancies with ONLY
    // 1 bit set.

    // North-east
    Occ   = Remove_Bit(Make_Diag_NE(sq), sq);
    count = Bit_Count(Occ);
    
    while (count--)
    {
      LSB = Get_LSB(Occ);

      if (Square_To_BB(sq) != LSB)
        BISHOP_MOVES[sq][NORTH_EAST][LSB] = Gen_Bishop_Moves_slow(sq, LSB);

      Pop_Bit(Occ, LSB);
    }

    // South-east
    Occ   = Remove_Bit(Make_Diag_SE(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      MSB = Get_MSB(Occ);

      if (Square_To_BB(sq) != MSB)
        BISHOP_MOVES[sq][SOUTH_EAST][MSB] = Gen_Bishop_Moves_slow(sq, MSB);

      Pop_Bit(Occ, MSB);
    }
    
    // South-west
    Occ   = Remove_Bit(Make_Diag_SW(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      MSB = Get_MSB(Occ);

      if (Square_To_BB(sq) != MSB)
        BISHOP_MOVES[sq][SOUTH_WEST][MSB] = Gen_Bishop_Moves_slow(sq, MSB);

      Pop_Bit(Occ, MSB);
    }

    // North-west
    Occ   = Remove_Bit(Make_Diag_NW(sq), sq);
    count = Bit_Count(Occ);

    while (count--)
    {
      LSB = Get_LSB(Occ);

      if (Square_To_BB(sq) != LSB)
        BISHOP_MOVES[sq][NORTH_WEST][LSB] = Gen_Bishop_Moves_slow(sq, LSB);

      Pop_Bit(Occ, LSB);
    }
    
    for (auto Dir : {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST})
      BISHOP_MOVES[sq][Dir][0] = Gen_Bishop_Moves_slow(sq, 0);
  }
}

void Init_Slider_Tables()
{
  assertm(DIRECTION_TBLS_INITIALISED, 
    "Direction tables must be created before slider tables!\
     - (Init Slider tables func)");

  SLIDER_PC_TBLS_INITIALISED = true;

  Init_Rook_Tables();
  Init_Bishop_Tables();
}

BB_Value Gen_Rook_Moves_fast(Square sq, BB_Value Occ)
{
  assertm(SLIDER_PC_TBLS_INITIALISED,
    "Slider tables must becreated before fast rook moves! -\
    (Fast Rook moves func)");

  BB_Value moves;
  moves = ROOK_MOVES[sq][NORTH][Get_LSB(FILES_N[sq] & Occ)] &
	  ROOK_MOVES[sq][EAST][Get_MSB(RANKS_E[sq] & Occ)] &
	  ROOK_MOVES[sq][SOUTH][Get_MSB(FILES_S[sq] & Occ)] &
	  ROOK_MOVES[sq][WEST][Get_LSB(RANKS_W[sq] & Occ)];

  return moves;
}

BB_Value Gen_Bishop_Moves_fast(Square sq, BB_Value Occ)
{
  assertm(SLIDER_PC_TBLS_INITIALISED,
    "Slider tables must becreated before fast bishop moves! -\
    (Fast Bishop moves func)");

  BB_Value moves;
  
  moves = 
   BISHOP_MOVES[sq][NORTH_EAST][Get_LSB(DIAGS_NE[sq] & Occ)] &
   BISHOP_MOVES[sq][SOUTH_EAST][Get_MSB(DIAGS_SE[sq] & Occ)] &
   BISHOP_MOVES[sq][SOUTH_WEST][Get_MSB(DIAGS_SW[sq] & Occ)] &
   BISHOP_MOVES[sq][NORTH_WEST][Get_LSB(DIAGS_NW[sq] & Occ)];


  return moves;
}

BB_Value Gen_Queen_Moves_fast(Square sq, BB_Value Occ)
{
  return Gen_Rook_Moves_fast(sq, Occ) |
	 Gen_Bishop_Moves_fast(sq, Occ);
}
