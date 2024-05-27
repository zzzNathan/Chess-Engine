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

#include <iostream>
#include <algorithm>
#include "Bit_Macros.hpp"

// Removes a given bit from a bitboard
BB_Value Remove_Bit(BB_Value bb, Square sq) {
  return bb & (~Square_To_BB(sq));
}

BB_Value Remove_Bit(BB_Value bb, BB_Value bit) {
  return bb & (~bit);
}

// Sets a given bit to a 1
BB_Value Set_Bit(BB_Value bb, Square sq) {
  return bb | Square_To_BB(sq);
}

BB_Value Set_Bit(BB_Value bb, BB_Value bit) {
  return bb | bit;
}

// Returns a bitboard of the square shifted up by 1 
// (Only from white's perspective)
BB_Value North(BB_Value bb) {
  return bb << 8;
}

BB_Value North(Square sq) {
  return Square_To_BB(sq) << 8;
}

// Returns a bitboard of the square shifted up and to the right by 1
// (Only from white's perspective)
BB_Value North_East(BB_Value bb) {
  return bb << 7;
}

BB_Value North_East(Square sq) {
  return Square_To_BB(sq) << 7;
}

// Returns a bitboard of the square shifted right by 1
// (Only from white's perspective)
BB_Value East(BB_Value bb) {
  return bb >> 1;
}

BB_Value East(Square sq) {
  return Square_To_BB(sq) >> 1;
}

// Returns a bitboard of the square shifted down and to the right by 1
// (Only from white's perspective)
BB_Value South_East(BB_Value bb) {
  return bb >> 9;
}

BB_Value South_East(Square sq) {
  return Square_To_BB(sq) >> 9;
}

// Returns a bitboard of the square shifted down by 1
// (Only from white's perspective)
BB_Value South(BB_Value bb) {
  return bb >> 8;
}

BB_Value South(Square sq) {
  return Square_To_BB(sq) >> 8;
}

// Returns a bitboard of the square shifted down and to the left by 1
// (Only from white's perspective)
BB_Value South_West(BB_Value bb) {
  return bb >> 7;
}

BB_Value South_West(Square sq) {
  return Square_To_BB(sq) >> 7;
}

// Returns a bitboard of the square shifted left by 1
// (Only from white's perspective)
BB_Value West(BB_Value bb) {
  return bb << 1;
}

BB_Value West(Square sq) {
  return Square_To_BB(sq) << 1;
}

// Returns a bitboard of the square shifted up and to the left by 1
// (Only from white's perspective)
BB_Value North_West(BB_Value bb) {
  return bb << 9;
}

BB_Value North_West(Square sq) {
  return Square_To_BB(sq) << 9;
}

// Makes a bitboard of all the squares north of a given square
BB_Value Make_File_North(Square sq)
{
  BB_Value File     = 0;
  Square Current_sq = sq;

  do 
  {
    File       = Set_Bit(File, Current_sq);
    Current_sq = SQ_North(Current_sq);

  } while (Current_sq <= a8);

  return File;
}

// Makes a bitboard of all the squares south of a given square
BB_Value Make_File_South(Square sq)
{
  BB_Value File     = 0;
  Square Current_sq = sq;

  do 
  {
    File       = Set_Bit(File, Current_sq);
    Current_sq = SQ_South(Current_sq);

  } while (Current_sq >= h1);

  return File;
}

// Makes a bitboard representing a specific file
BB_Value Make_File(Square file) {
  return Make_File_North(file) | Make_File_South(file);
}

// Makes a bitboard of all squares east of a given square
BB_Value Make_Rank_East(Square sq)
{
  BB_Value Rank     = 0;
  Square Current_sq = sq;

  int dist = Dist_To_East(sq) + 1;

  while (dist--)
  {
    Rank       = Set_Bit(Rank, Current_sq);
    Current_sq = SQ_East(Current_sq);
  }

  return Rank;
}

// Makes a bitboard of all squares west of a given square
BB_Value Make_Rank_West(Square sq)
{
  BB_Value Rank     = 0;
  Square Current_sq = sq;

  int dist = Dist_To_West(sq) + 1;

  while (dist--)
  {
    Rank       = Set_Bit(Rank, Current_sq);
    Current_sq = SQ_West(Current_sq);
  }

  return Rank;
}

// Makes a bitboard representing a specific rank
BB_Value Make_Rank(Square rank) {
  return Make_Rank_East(rank) | Make_Rank_West(rank);  
}

// Makes a bitboard of all squares north-east of a given square
BB_Value Make_Diag_NE(Square sq)
{
  BB_Value Diag     = 0;
  Square Current_sq = sq;

  int dist = std::min(Dist_To_North(sq), Dist_To_East(sq)) + 1;

  while (dist--)
  {
    Diag       = Set_Bit(Diag, Current_sq);
    Current_sq = SQ_North_East(Current_sq);
  }
  
  return Diag;
}

// Makes a bitboard of all squares south-east of a given square
BB_Value Make_Diag_SE(Square sq)
{
  BB_Value Diag     = 0;
  Square Current_sq = sq;

  int dist = std::min(Dist_To_South(sq), Dist_To_East(sq)) + 1;

  while (dist--)
  {
    Diag       = Set_Bit(Diag, Current_sq);
    Current_sq = SQ_South_East(Current_sq);
  }

  return Diag;
}

// Makes a bitboard of all squares south-west of a given square
BB_Value Make_Diag_SW(Square sq)
{
  BB_Value Diag     = 0;
  Square Current_sq = sq;

  int dist = std::min(Dist_To_South(sq), Dist_To_West(sq)) + 1;

  while (dist--)
  {
    Diag       = Set_Bit(Diag, Current_sq);
    Current_sq = SQ_South_West(Current_sq);
  }

  return Diag;
}

// Makes a bitboard of all squares south-west of a given square
BB_Value Make_Diag_NW(Square sq)
{
  BB_Value Diag     = 0;
  Square Current_sq = sq;

  int dist = std::min(Dist_To_North(sq), Dist_To_West(sq)) + 1;

  while (dist--)
  {
    Diag       = Set_Bit(Diag, Current_sq);
    Current_sq = SQ_North_West(Current_sq);
  }

  return Diag;
}

// Makes a bitboard representing a specific diagonal
BB_Value Make_Diag(Square diag) {
  return Make_Diag_NE(diag) | Make_Diag_SE(diag) |
	 Make_Diag_SW(diag) | Make_Diag_NW(diag);
}

// Debugging function used to visualise a bitboard value
void Show_BB(BB_Value bb)
{
  std::cout << bb << "\n";

  // Loop over all squares & if the bit representing this
  // square is set print a '1' otherwise print a '.'
  for (Square sq = a8; sq >= h1; sq--)
  {
    if (Square_To_BB(sq) & bb) std::cout << "1 ";
    else std::cout << ". ";

    // Print a new line once we have finished a row
    if (Is_Right_edge(sq)) std::cout << "\n";
  }

  std::cout << "\n";
}
