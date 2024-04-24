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
#ifndef BITMACROS_HPP
#define BITMACROS_HPP
#include <cmath>
#include <cstdint>

using namespace std;

typedef unsigned long long Bitboard;
typedef short              Index;

// Bitboards are the actual 64 bit integers representing piece locations
// Indexes are the numbers from [0, 63] representing squares, see diagram
// below.

// Function to reverse the binary representation of a 64 bit integer 
// CREDITS: https://chess.stackexchange.com/questions/37309/move-generation-for-sliding-pieces-and-hyperbola-quintessence
inline Bitboard Reverse_bits(Bitboard Board)
{
  Board = ((Board & 0x5555555555555555) << 1) | ((Board >> 1) & 0x5555555555555555);
  Board = ((Board & 0x3333333333333333) << 2) | ((Board >> 2) & 0x3333333333333333);
  Board = ((Board & 0x0f0f0f0f0f0f0f0f) << 4) | ((Board >> 4) & 0x0f0f0f0f0f0f0f0f);
  Board = ((Board & 0x00ff00ff00ff00ff) << 8) | ((Board >> 8) & 0x00ff00ff00ff00ff);

  return (Board << 48) | ((Board & 0xffff0000) << 16) | ((Board >> 16) & 0xffff0000) | (Board >> 48);
}

inline Bitboard Get_Bit(const Bitboard& Board, const Index& Square){
  return Board & (1ULL << Square);
}

inline Bitboard Set_Bit(const Bitboard& Board, const Index& Square){
  return Board | (1ULL << Square);
}

inline Bitboard Get_LSB(const Bitboard& Board){
  if (Board == 0) return 0ULL; 
  return Board & (~Board + 1);
}

inline Bitboard Get_Index(const Bitboard& Board){
  if (Board == 0) return 0ULL;
  return (Bitboard) log2(Board);
}

inline Bitboard Index_To_Bitboard(const Index& Square){
  return 1ULL << Square;
}

inline Bitboard Remove_Bit(const Bitboard& Board, const Index& Square){
  return Board & ~(1ULL << Square);
}

// Count the number of bits set to 1 in a 64 bit integer
inline Bitboard BitCount(const Bitboard& Board){
  return __builtin_popcountll(Board);
}
#endif
