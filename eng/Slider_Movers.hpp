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

#ifndef SLIDER_MOVERS_HPP
#define SLIDER_MOVERS_HPP

#include "Constants.hpp"

BB_Value Gen_Rook_Moves_slow(Square sq, BB_Value Occupancy);
BB_Value Gen_Bishop_Moves_slow(Square sq, BB_Value Occupancy);
BB_Value Gen_Queen_Moves_slow(Square sq, BB_Value Occupancy);

#endif
