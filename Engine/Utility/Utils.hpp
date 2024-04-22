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
#include "Constants.hpp"
#include <map>
#include <iostream>

using namespace std;

// This function will take in some character of an number
// like '5' and return the number 5 as an integer.
// When we do -'0' the character is implicitly converted
// to it's integer ascii value. 
inline int Char_To_Int(const char& n){
  return n - '0';
}

// Shift a single bit bitboard up one
inline Bitboard Shift_Up(const Bitboard& Board, const bool& Colour){
  return Colour == WHITE ? (Board << 8) : (Board >> 8);
}

// Shift a single bit bitboard down one
inline Bitboard Shift_Down(const Bitboard& Board, const bool& Colour){
  return Colour == WHITE ? (Board >> 8) : (Board << 8);
}

// Shifts a square index left
inline Index Index_Left(const Index& Square){
  return Square + 1;
}

// Shifts a square index left by 2
inline Index Index_Left_2(const Index& Square){
  return Square + 2;
}

// Shifts a square index right
inline Index Index_Right(const Index& Square){
  return Square - 1;
}

// Shifts a square index right by 2
inline Index Index_Right_2(const Index& Square){
  return Square - 2;
}

// Shifts a square index down
inline Index Index_Down(const Index& Square, const bool& Colour){
  return Colour == WHITE ? Square - 8 : Square + 8;
}

// Shifts a square index down by 2 
inline Index Index_Down_2(const Index& Square, const bool& Colour){
  return Colour == WHITE ? Square - 16 : Square + 16;
}

// Shifts a square index up
inline Index Index_Up(const Index& Square, const bool& Colour){
  return Colour == WHITE ? Square + 8 : Square - 8;
}

// Shifts a square index up by 2
inline Index Index_Up_2(const Index& Square, const bool& Colour){
  return Colour == WHITE ? Square + 16 : Square - 16;
}

void Show_Bitboard(const Bitboard& Board);
map<pair<Bitboard, Bitboard>, Bitboard> Make_Ray_Map();
Bitboard Create_Ray(const Bitboard& From, const Bitboard& To);
