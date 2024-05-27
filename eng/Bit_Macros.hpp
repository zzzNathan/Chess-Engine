/*
    Evergreen: A didactic C++ chess engine 
    Copyright (C) 2024  Jonathan Kasongo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BIT_MACROS_HPP
#define BIT_MACROS_HPP

#include "Core.hpp"

// Bitboard manipulation functions
BB_Value Remove_Bit(BB_Value bb, Square sq);
BB_Value Remove_Bit(BB_Value bb, BB_Value bit);
BB_Value Set_Bit(BB_Value File_BB, Square Current_sq); 
BB_Value Set_Bit(BB_Value File_BB, BB_Value bit); 

BB_Value North(BB_Value bb);
BB_Value North_East(BB_Value bb);
BB_Value East(BB_Value bb);
BB_Value South_East(BB_Value bb);
BB_Value South(BB_Value bb);
BB_Value South_West(BB_Value bb);
BB_Value West(BB_Value bb);
BB_Value North_West(BB_Value bb);

BB_Value North(Square sq);
BB_Value North_East(Square sq);
BB_Value East(Square sq);
BB_Value South_East(Square sq);
BB_Value South(Square sq);
BB_Value South_West(Square sq);
BB_Value West(Square sq);
BB_Value North_West(Square sq);

BB_Value Make_File_North(Square sq);
BB_Value Make_File_South(Square sq);
BB_Value Make_File(Square file);  

BB_Value Make_Rank_East(Square sq);
BB_Value Make_Rank_West(Square sq);
BB_Value Make_Rank(Square rank);

BB_Value Make_Diag_NE(Square sq);
BB_Value Make_Diag_SE(Square sq);
BB_Value Make_Diag_SW(Square sq);
BB_Value Make_Diag_NW(Square sq);
BB_Value Make_Diag(Square diag);

void Show_BB(BB_Value);

#endif 
