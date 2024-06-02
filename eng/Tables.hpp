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

#ifndef TABLES_HPP
#define TABLES_HPP

#include "Slider_Movers.hpp"

void Init_Rook_Tables();
void Init_Bishop_Tables();
void Init_Slider_Tables();

extern std::unordered_map<BB_Value, BB_Value> ROOK_MOVES[64][4];
extern std::unordered_map<BB_Value, BB_Value> BISHOP_MOVES[64][4];

BB_Value Gen_Rook_Moves_fast();
BB_Value Gen_Bishop_Moves_fast();
BB_Value Gen_Queen_Moves_fast();

#endif
