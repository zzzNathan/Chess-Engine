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
#include "Utils.hpp"

// Function to print a visualisation of a bitboard
void Show_Bitboard(const Bitboard& Board)
{
  cout.tie(nullptr); // Faster output
  
  short rank = 8;
  cout << rank << " | ";

  for (Bitboard sq = a8; sq != h1; sq--)
  {
    // If the square is set print 1 else 0
    if (Get_Bit(Board, sq)) cout << "1 ";
    else cout << "0 ";

    // Every 8 squares start a new line
    if (sq % 8 == 0) cout << "\n" << --rank << " | "; 
  }

  // Print last square
  if (Get_Bit(Board, 0)) cout << "1\n";
  else cout << "0\n";

  // Print letters
  cout << "    _______________\n";
  cout << "    a b c d e f g h\n\n";
}

// Function to make a map that takes in 2 squares and returns either a 
// horizontal or diagonal ray from these 2 squares
map<pair<Bitboard, Bitboard>, Bitboard> Make_Ray_Map()
{
  map<pair<Bitboard, Bitboard>, Bitboard> Ray_Map;

  // Directions we can travel in (see Constants.h - line 27)
  const static int Directions[8] = {1, -1, 7, -7, 8, -8, 9, -9}; 
  
  // Lambda function to check whether or not we are able to move in this direction
  auto Can_Move = [](Bitboard From, int delta)
  {
    // Cant move right on the right edge of board  
    if ((From % 8 == 0) && (delta == -1 || delta ==  7 || delta == -9)) return false; 

    // Cant move left on the left edge of board
    if ((From % 8 == 7) && (delta ==  1 || delta == -7 || delta ==  9)) return false;

    // Cant move up on the top edge of board
    if ((From >= 56 && From <= 63) && (delta ==  8 || delta ==  7 || delta ==  9)) return false;

    // Cant move down on the bottom edge of board
    if ((From >=  0 && From <=  7) && (delta == -8 || delta == -7 || delta == -9)) return false;

    return true; // Otherwise we can move in this direction
  };

  // Loop over all squares
  Bitboard Curr_Sq, Ray;
  Bitboard Curr_Sq_BB, sq_BB;

  for (int sq = a8; sq >= h1; sq--)
    // Loop over all directions
    for (int delta : Directions)
    {
      // Keep moving in this direction until we hit the board edge
      Curr_Sq = sq;
      Ray     = Index_To_Bitboard(sq);

      while (Can_Move(Curr_Sq, delta))
      {
        Curr_Sq += delta;
        Ray      = Set_Bit(Ray, Curr_Sq);

        // Then map the pair of squares to their relevant ray
        Curr_Sq_BB = Index_To_Bitboard(Curr_Sq);
        sq_BB      = Index_To_Bitboard(sq);
        Ray_Map[{sq_BB, Curr_Sq_BB}] = Ray;
      }
    }
  
  return Ray_Map;
}

// Function to make a horizontal or diagonal ray from one square to another
Bitboard Create_Ray(const Bitboard& From, const Bitboard& To)
{
  // Handle edge cases
  if (From == To || From == NoBits || To == NoBits) return 0;

  // Initialise the ray map
  const static map<pair<Bitboard, Bitboard>, Bitboard> Ray_Map = Make_Ray_Map();

  // Check if the ray is in the map
  if (Ray_Map.find({From, To}) != Ray_Map.end()) return Ray_Map.at({From, To}); 

  return 0; // Otherwise return no ray
}
