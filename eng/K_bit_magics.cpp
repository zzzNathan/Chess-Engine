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

#include "Slider_Movers.hpp"

// Get next greater value with same number of 1 bits
// CREDITS: Originally appeared as HAKMEM ITEM 175 (Bill Gosper)
BB_Value Snoob(BB_Value x)
{
   BB_Value smallest, ripple, ones;

   smallest = x & -x;
   ripple   = x + smallest;
   ones     = x ^ ripple;
   ones     = (ones >> 2) / smallest;

   return ripple | ones;
}

std::unordered_map<BB_Value, BB_Value> ROOK_MOVES;
std::unordered_map<BB_Value, BB_Value> BISHOP_MOVES;
