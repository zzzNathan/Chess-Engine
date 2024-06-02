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

// Bitboards are the actual 64 bit unsigned integers representing
// piece locations. Each bit represents one square and it can be
// visualised below. If the bit is set to 1 it indicates that a 
// piece exists on that square & if the bit is set to 0 it 
// indicates that no piece exists on that square.

/*
  The board may be represented like so:

     A    B    C    D    E    F    G    H
   +----+----+----+----+----+----+----+----+
 8 | 63 | 62 | 61 | 60 | 59 | 58 | 57 | 56 |  8th rank
   +----+----+----+----+----+----+----+----+
 7 | 55 | 54 | 53 | 52 | 51 | 50 | 49 | 48 |  7th rank
   +----+----+----+----+----+----+----+----+
 6 | 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 |  6th rank
   +----+----+----+----+----+----+----+----+
 5 | 39 | 38 | 37 | 36 | 35 | 34 | 33 | 32 |  5th rank
   +----+----+----+----+----+----+----+----+
 4 | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 24 |  4th rank
   +----+----+----+----+----+----+----+----+
 3 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 |  3rd rank
   +----+----+----+----+----+----+----+----+
 2 | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  2nd rank
   +----+----+----+----+----+----+----+----+
 1 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  1st rank
   +----+----+----+----+----+----+----+----+
     A    B    C    D    E    F    G    H - file(s)
________________________________________________________
credits = https://www.chessprogramming.org/Little-endian
*/

// We define a square to be the log base 2 of the square on 
// a bitboard. For example the square e4 is represented by
// the index 27. (See diagram above).

#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <bit>
#include <cassert>

// Allows messages upon assertion failure
#define assertm(exp, msg) assert(((void)msg, exp))

typedef short Square; 
typedef unsigned long long BB_Value;

enum Index : Square 
{
  h1, g1, f1, e1, d1, c1, b1, a1,
  h2, g2, f2, e2, d2, c2, b2, a2,
  h3, g3, f3, e3, d3, c3, b3, a3,
  h4, g4, f4, e4, d4, c4, b4, a4,
  h5, g5, f5, e5, d5, c5, b5, a5,
  h6, g6, f6, e6, d6, c6, b6, a6,
  h7, g7, f7, e7, d7, c7, b7, a7,
  h8, g8, f8, e8, d8, c8, b8, a8,
  no_sq
};

enum Directions_Vertical_Horizontal : int
{
  NORTH, EAST, SOUTH, WEST, 
};

enum Directions_Diagonal : int
{
  NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST
};

class Bitboard
{
  public:
    Bitboard(BB_Value data);

    // Methods
    void Show_Bitboard() const;

    // Data members
    BB_Value value;
};

// Square manipulation functions
Square SQ_North(Square sq);
Square SQ_North_East(Square sq);
Square SQ_East(Square sq);
Square SQ_South_East(Square sq);
Square SQ_South(Square sq);
Square SQ_South_West(Square sq);
Square SQ_West(Square sq);
Square SQ_North_West(Square sq);

int Dist_To_North(Square sq);
int Dist_To_East(Square sq);
int Dist_To_South(Square sq);
int Dist_To_West(Square sq);

BB_Value Square_To_BB(Square sq);
Square BB_To_Square(BB_Value bb);

bool Is_Right_edge(Square sq);

#endif
