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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <unordered_map>
#include "Core.hpp"
#include "Bit_Macros.hpp"

// Have the relevant tables been initialised?
extern bool DIRECTION_TBLS_INITIALISED;
extern bool STATIC_PC_TBLS_INITIALISED;
extern bool SLIDER_PC_TBLS_INITIALISED;

// Arrays/maps of masks for all files and ranks. A mask is a
// bitboard with all the bits in the respective file/rank 
// set to 1.

// The NOT arrays/maps are the same as the arrays above them
// just that every mask, x is now the bitwise NOT of x.
extern BB_Value FILES_N[64];
extern BB_Value FILES_S[64];
extern BB_Value FILES[64];
extern BB_Value NOT_FILES[64];

extern BB_Value RANKS_E[64];
extern BB_Value RANKS_W[64];
extern BB_Value RANKS[64];
extern BB_Value NOT_RANKS[64];

extern BB_Value DIAGS_NE[64];
extern BB_Value DIAGS_SE[64];
extern BB_Value DIAGS_SW[64];
extern BB_Value DIAGS_NW[64];
extern BB_Value DIAGS[64];

extern const Square file_A;  extern const Square rank_1;
extern const Square file_B;  extern const Square rank_2;
extern const Square file_C;  extern const Square rank_3;
extern const Square file_D;  extern const Square rank_4;
extern const Square file_E;  extern const Square rank_5;
extern const Square file_F;  extern const Square rank_6;
extern const Square file_G;  extern const Square rank_7;
extern const Square file_H;  extern const Square rank_8;

void Init_Files();
void Init_Ranks();
void Init_Diags();
void Init_Direction_Tables();

extern const bool BLOCKER;
extern const bool NON_BLOCKER;

#endif
