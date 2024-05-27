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

bool DIRECTION_TBLS_INITIALISED = false;
bool STATIC_PC_TBLS_INITIALISED = false;
bool SLIDER_PC_TBLS_INITIALISED = false;

BB_Value FILES_N[64];
BB_Value FILES_S[64];
BB_Value FILES[64];
BB_Value NOT_FILES[64];

BB_Value RANKS_E[64];
BB_Value RANKS_W[64];
BB_Value RANKS[64];
BB_Value NOT_RANKS[64];

BB_Value DIAGS_NE[64];
BB_Value DIAGS_SE[64];
BB_Value DIAGS_SW[64];
BB_Value DIAGS_NW[64];
BB_Value DIAGS[64];

void Init_Files()
{
  for (Square sq = h1; sq <= a8; sq++) 
  {
    FILES_N[sq]   = Make_File_North(sq);
    FILES_S[sq]   = Make_File_South(sq);
    FILES[sq]     = Make_File(sq);
    NOT_FILES[sq] = ~FILES[sq];
  }
}

void Init_Ranks()
{
  for (Square sq = h1; sq <= a8; sq++)
  {
    RANKS_E[sq]   = Make_Rank_East(sq);
    RANKS_W[sq]   = Make_Rank_West(sq);
    RANKS[sq]     = Make_Rank(sq);
    NOT_RANKS[sq] = ~RANKS[sq];
  }
}

void Init_Diags()
{
  for (Square sq = h1; sq <= a8; sq++)
  {
    DIAGS_NE[sq] = Make_Diag_NE(sq);
    DIAGS_SE[sq] = Make_Diag_SE(sq);
    DIAGS_SW[sq] = Make_Diag_SW(sq);
    DIAGS_NW[sq] = Make_Diag_NW(sq);
    DIAGS[sq]    = Make_Diag(sq);
  }
}

void Init_Direction_Tables()
{
  DIRECTION_TBLS_INITIALISED = true;
  Init_Files();
  Init_Ranks();
  Init_Diags();
}

// Indicators for files and ranks. Files are accessed using the
// bottom-most square, ranks are accessed using the left-most
// square.
const Square file_A = a1;  const Square rank_1 = a1;
const Square file_B = b1;  const Square rank_2 = a2;
const Square file_C = c1;  const Square rank_3 = a3;
const Square file_D = d1;  const Square rank_4 = a4;
const Square file_E = e1;  const Square rank_5 = a5;
const Square file_F = f1;  const Square rank_6 = a6;
const Square file_G = g1;  const Square rank_7 = a7;
const Square file_H = h1;  const Square rank_8 = a8;
