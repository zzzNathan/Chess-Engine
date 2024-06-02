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

// A collection of core modules needed for other files to run

#include "Core.hpp"

Square SQ_North(Square sq) {
  return sq + 8;
}

Square SQ_North_East(Square sq) {
  return sq + 7;
}

Square SQ_East(Square sq) {
  return sq - 1;
}

Square SQ_South_East(Square sq) {
  return sq - 9;
}

Square SQ_South(Square sq) {
  return sq - 8;
}

Square SQ_South_West(Square sq) {
  return sq - 7;
}

Square SQ_West(Square sq) {
  return sq + 1;
}

Square SQ_North_West(Square sq) {
  return sq + 9;
}

// Takes in a square and returns a 64 bit unsigned integer
BB_Value Square_To_BB(Square sq) {
  return 1ULL << sq;
}

// Takes in a bitboard and returns the square it represents
// (NOTE: We must compile with C++ 20 or higher for the
// "countr_zero" function to run)
Square BB_To_Square(BB_Value bb) {
  assertm(bb != 0, 
    "Bitboard value can't be 0! - (BB_To_Square func)");

  // The number of leading 0s is the same as taking log 
  // base 2, if we know that 'bb' only has 1 bit set to a 1
  return std::countr_zero(bb);  
}

// Returns the distance from a square to a board edge
int Dist_To_North(Square sq) {
  return (63 - sq) / 8;
}

int Dist_To_East(Square sq) {
  return sq % 8;
}

int Dist_To_South(Square sq) {
  return sq / 8;
}

int Dist_To_West(Square sq) {
  return 7 - (sq % 8);
}

// A square is on the right edge of the board if it's a multiple of
// 8
bool Is_Right_edge(Square sq) {
  return sq % 8 == 0;
}

Bitboard::Bitboard(BB_Value data)
{
  value = data;
}

// Debugging function used to visualise a bitboard
void Bitboard::Show_Bitboard() const
{
  // Loop over all squares & if the bit representing this 
  // square is set print a '1' otherwise print a '.'
  for (Square sq = a8; sq >= h1; sq--)
  {
    if (Square_To_BB(sq) & value) std::cout << "1 ";
    else std::cout << ". ";

    // Print a new line once we have finished a row
    if (Is_Right_edge(sq)) std::cout << "\n";
  }

  std::cout << "\n";
}
