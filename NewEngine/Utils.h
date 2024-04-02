/*
    Shallow-Thought: A didactic C++ chess engine 
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
#include "Constants.h"
#include <unordered_map>
#include <iostream>
using namespace std;
typedef unsigned long long i64;

bool Is_First_Rank(i64 Bitboard){
  return (Bitboard & Rank1) == Rank1;
}

bool Is_Second_Rank(i64 Bitboard){
  return (Bitboard & Rank2) == Rank2;
}

bool Is_Seventh_Rank(i64 Bitboard){
  return (Bitboard & Rank7) == Rank7;
}

bool Is_Eighth_Rank(i64 Bitboard){
  return (Bitboard & Rank8) == Rank8;
}

// Function to print a visualisation of a bitboard
void Show_Bitboard(i64 Bitboard){
  for (i64 sq = 63; sq != 0; sq--){

    // If the square is set print 1 else 0
    if (Get_Bit(Bitboard, sq)) cout << "1 ";
    else cout << "0 ";

    // Every 8 squares start a new line
    if (sq % 8 == 0) cout << "\n";
  }

  // Print last square
  if (Get_Bit(Bitboard, 0)) cout << "1";
  else cout << "0";
  cout << "\n";
}
