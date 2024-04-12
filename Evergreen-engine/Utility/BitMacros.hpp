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
#include <cmath>
using namespace std;
typedef unsigned long long i64;

// Function to reverse the binary representation of a 64 bit integer 
// CREDITS: https://chess.stackexchange.com/questions/37309/move-generation-for-sliding-pieces-and-hyperbola-quintessence
i64 Reverse_bits(i64 BB)
{
  BB = ((BB & 0x5555555555555555) << 1) | ((BB >> 1) & 0x5555555555555555);
  BB = ((BB & 0x3333333333333333) << 2) | ((BB >> 2) & 0x3333333333333333);
  BB = ((BB & 0x0f0f0f0f0f0f0f0f) << 4) | ((BB >> 4) & 0x0f0f0f0f0f0f0f0f);
  BB = ((BB & 0x00ff00ff00ff00ff) << 8) | ((BB >> 8) & 0x00ff00ff00ff00ff);

  return (BB << 48) | ((BB & 0xffff0000) << 16) | ((BB >> 16) & 0xffff0000) | (BB >> 48);
}

inline i64 Get_Bit(const i64& Bitboard, const i64& Square){
  return Bitboard & (1ULL << Square);
}

inline i64 Set_Bit(const i64& Bitboard, const i64& Square){
  return Bitboard | (1ULL << Square);
}

inline i64 Get_LSB(const i64& Bitboard){
  if (Bitboard == 0) return 0ULL; 
  return Bitboard & (~Bitboard + 1);
}

inline i64 Get_Index(const i64& Bitboard){
  if (Bitboard == 0) return 0ULL;
  return (i64) log2(Bitboard);
}

inline i64 Index_To_Bitboard(const i64& Index){
  return 1ULL << Index;
}

inline i64 Remove_Bit(const i64& Bitboard, const i64& Square){
  return Bitboard & ~(1ULL << Square);
}

// Count the number of bits set to 1 in a 64 bit integer
inline i64 BitCount(const i64& Bitboard){
  return __builtin_popcountll(Bitboard);
}
