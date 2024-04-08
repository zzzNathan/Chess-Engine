/*
    Shallow-Thought: A didactic C++ chess engine 
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
#include "BitMacros.hpp"
#include <string>
#include <cstdint>
#include <array>
#include <unordered_map>
using namespace std;
typedef unsigned long long i64;

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

// Bitboard constants
// -------------------
const i64 AllBits = 0xffffffffffffffff; 
const i64 NoBits = 0x0;

// File constants
const i64 NotFileA = 0x7F7F7F7F7F7F7F7F;
const i64 FileA    = 0x8080808080808080;
const i64 NotFileB = 0xBFBFBFBFBFBFBFBF;
const i64 FileB    = 0x4040404040404040;
const i64 FileC    = 0x2020202020202020;
const i64 FileD    = 0x1010101010101010;
const i64 FileE    = 0x808080808080808;
const i64 FileF    = 0x404040404040404;
const i64 NotFileG = 0xFDFDFDFDFDFDFDFD;
const i64 FileG    = 0x202020202020202;
const i64 NotFileH = 0xFEFEFEFEFEFEFEFE;
const i64 FileH    = 0x101010101010101;
const i64 FILES[8] = {FileH, FileG, FileF, FileE, FileD, FileC, FileB, FileA};

// Rank constants
const i64 Rank8 = 0xFF00000000000000;
const i64 Rank7 = 0xFF000000000000;
const i64 Rank6 = 0xFF0000000000;
const i64 Rank5 = 0xFF00000000;
const i64 Rank4 = 0xFF000000;
const i64 Rank3 = 0xFF0000;
const i64 Rank2 = 0xFF00;
const i64 Rank1 = 0xFF;
const i64 RANKS[9] = {0, Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8};

// Diagonal constants
const i64 Diag_H1_H1 = 0x1;
const i64 Diag_G1_H2 = 0x102;
const i64 Diag_F1_H3 = 0x10204;
const i64 Diag_E1_H4 = 0x1020408;
const i64 Diag_D1_H5 = 0x102040810;
const i64 Diag_C1_H6 = 0x10204081020;
const i64 Diag_B1_H7 = 0x1020408102040;
const i64 Diag_A1_H8 = 0x102040810204080;
const i64 Diag_A2_G8 = 0x204081020408000;
const i64 Diag_A3_F8 = 0x408102040800000;
const i64 Diag_A4_E8 = 0x810204080000000;
const i64 Diag_A5_D8 = 0x1020408000000000;
const i64 Diag_A6_C8 = 0x2040800000000000;
const i64 Diag_A7_B8 = 0x4080000000000000;
const i64 Diag_A8_A8 = 0x8000000000000000;
const i64 DIAGS[64] = {  // An array mapping squares to their diagonals
    Diag_H1_H1, Diag_G1_H2, Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8,
    Diag_G1_H2, Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8,
    Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8,
    Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, 
    Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8,
    Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8,
    Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8, Diag_A7_B8,
    Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8, Diag_A7_B8, Diag_A8_A8
};

// Anti-diagonal constants
const i64 Diag_A1_A1 = 0x80;
const i64 Diag_B1_A2 = 0x8040;
const i64 Diag_C1_A3 = 0x804020;
const i64 Diag_D1_A4 = 0x80402010;
const i64 Diag_E1_A5 = 0x8040201008;
const i64 Diag_F1_A6 = 0x804020100804;
const i64 Diag_G1_A7 = 0x80402010080402;
const i64 Diag_H1_A8 = 0x8040201008040201;
const i64 Diag_H2_B8 = 0x4020100804020100;
const i64 Diag_H3_C8 = 0x2010080402010000;
const i64 Diag_H4_D8 = 0x1008040201000000;
const i64 Diag_H5_E8 = 0x804020100000000;
const i64 Diag_H6_F8 = 0x402010000000000;
const i64 Diag_H7_G8 = 0x201000000000000;
const i64 Diag_H8_H8 = 0x100000000000000;
const i64 ANTI_DIAGS[64] = {  // An array mapping squares to their anti-diagonals 
    Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3, Diag_B1_A2, Diag_A1_A1,
    Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3, Diag_B1_A2,
    Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3,
    Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4,
    Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5,
    Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6,
    Diag_H7_G8, Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7,
    Diag_H8_H8, Diag_H7_G8, Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8
};

// Square specific constants
const i64 SquareE1 = 8;
const i64 SquareE8 = 0x800000000000000;
const i64 SquareH1 = 1;
const i64 SquareF1 = 4;
const i64 SquareG1 = 2;
const i64 SquareG8 = 0x200000000000000; 
const i64 SquareH8 = 0x100000000000000;
const i64 SquareF8 = 0x400000000000000;
const i64 SquareA1 = 0x80;
const i64 SquareD1 = 0x10;
const i64 SquareA8 = 0x8000000000000000;
const i64 SquareD8 = 0x1000000000000000;
const i64 SquareC1 = 0x20;
const i64 SquareC8 = 0x2000000000000000;

// Castling indicators (see Utils.h)
const uint8_t W_Kingside = 0x1;
const uint8_t W_Queenside = 0x2;
const uint8_t B_Kingside = 0x4;
const uint8_t B_Queenside = 0x8;

// White and black indicators (see Game.h)
const bool WHITE = true;
const bool BLACK = false;

// Piece indicators (See Game.h)
const i64 PAWN   = 0;
const i64 KNIGHT = 1;
const i64 BISHOP = 2;
const i64 ROOK   = 3;
const i64 QUEEN  = 4;
const i64 KING   = 5;
const i64 NO_PROMO = 0;

// Null indicator (see Game.h)
const i64 NONE = AllBits;

// Maximum ply until the game is a draw
const uint8_t MAX_PLY = 100;

// Mapping squares to their indexes
enum {
  h1, g1, f1, e1, d1, c1, b1, a1,
  h2, g2, f2, e2, d2, c2, b2, a2,
  h3, g3, f3, e3, d3, c3, b3, a3,
  h4, g4, f4, e4, d4, c4, b4, a4,
  h5, g5, f5, e5, d5, c5, b5, a5,
  h6, g6, f6, e6, d6, c6, b6, a6,
  h7, g7, f7, e7, d7, c7, b7, a7,
  h8, g8, f8, e8, d8, c8, b8, a8
};

// Mapping squares names to their indexes
const unordered_map<string, i64> Square_To_Index = {
 {"h1", h1}, {"g1", g1}, {"f1", f1}, {"e1", e1}, {"d1", d1}, {"c1", c1}, {"b1", b1}, {"a1", a1},
 {"h2", h2}, {"g2", g2}, {"f2", f2}, {"e2", e2}, {"d2", d2}, {"c2", c2}, {"b2", b2}, {"a2", a2},
 {"h3", h3}, {"g3", g3}, {"f3", f3}, {"e3", e3}, {"d3", d3}, {"c3", c3}, {"b3", b3}, {"a3", a3},
 {"h4", h4}, {"g4", g4}, {"f4", f4}, {"e4", e4}, {"d4", d4}, {"c4", c4}, {"b4", b4}, {"a4", a4},
 {"h5", h5}, {"g5", g5}, {"f5", f5}, {"e5", e5}, {"d5", d5}, {"c5", c5}, {"b5", b5}, {"a5", a5},
 {"h6", h6}, {"g6", g6}, {"f6", f6}, {"e6", e6}, {"d6", d6}, {"c6", c6}, {"b6", b6}, {"a6", a6},
 {"h7", h7}, {"g7", g7}, {"f7", f7}, {"e7", e7}, {"d7", d7}, {"c7", c7}, {"b7", b7}, {"a7", a7},
 {"h8", h8}, {"g8", g8}, {"f8", f8}, {"e8", e8}, {"d8", d8}, {"c8", c8}, {"b8", b8}, {"a8", a8}
};

// Mapping square indexes to their names
const string Index_To_Square[64] = {
  "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
  "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
  "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
  "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
  "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
  "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
  "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
  "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};

// Mapping pieces to their UCI names
const string Piece_To_Uci[5] = {
  "", "n", "b", "r", "q"
};

// Chess fen strings
const string STARTING_FEN    = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const string TRICKY_POSITION = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
