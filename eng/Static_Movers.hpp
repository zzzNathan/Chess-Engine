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

// This file aims to pre-compute moves for the static pieces that
// is, King, Pawn & Knight. Possible moves are indicated by 1's
// on a bitboard.

#ifndef STATIC_MOVERS_HPP
#define STATIC_MOVERS_HPP

#include "Constants.hpp"

BB_Value Gen_King_Moves(Square sq);
BB_Value Gen_W_Pawn_Moves(Square sq);
BB_Value Gen_B_Pawn_Moves(Square sq);
BB_Value Gen_W_Pawn_Atks(Square sq);
BB_Value Gen_B_Pawn_Atks(Square sq);
BB_Value Gen_Knight_Moves(Square sq);

void Init_Static_Piece_Tables();

#endif
