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
#include "BitMacros.hpp"
#include <string>
#include <unordered_map>

using namespace std;

// Bitboards are the actual 64 bit integers representing piece locations
// Indexes are the numbers from [0, 63] representing squares, see diagram
// below.

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
const Bitboard AllBits = 0xffffffffffffffff; 
const Bitboard NoBits  = 0x0;

// File constants
const Bitboard NotFileA = 0x7F7F7F7F7F7F7F7F;
const Bitboard FileA    = 0x8080808080808080;
const Bitboard NotFileB = 0xBFBFBFBFBFBFBFBF;
const Bitboard FileB    = 0x4040404040404040;
const Bitboard FileC    = 0x2020202020202020;
const Bitboard FileD    = 0x1010101010101010;
const Bitboard FileE    = 0x808080808080808;
const Bitboard FileF    = 0x404040404040404;
const Bitboard NotFileG = 0xFDFDFDFDFDFDFDFD;
const Bitboard FileG    = 0x202020202020202;
const Bitboard NotFileH = 0xFEFEFEFEFEFEFEFE;
const Bitboard FileH    = 0x101010101010101;
const Bitboard FILES[8] = {FileH, FileG, FileF, FileE, FileD, FileC, FileB, FileA};

// Rank constants
const Bitboard Rank8 = 0xFF00000000000000;
const Bitboard Rank7 = 0xFF000000000000;
const Bitboard Rank6 = 0xFF0000000000;
const Bitboard Rank5 = 0xFF00000000;
const Bitboard Rank4 = 0xFF000000;
const Bitboard Rank3 = 0xFF0000;
const Bitboard Rank2 = 0xFF00;
const Bitboard Rank1 = 0xFF;
const Bitboard RANKS[9] = {0, Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8};

// Diagonal constants
const Bitboard Diag_H1_H1 = 0x1;
const Bitboard Diag_G1_H2 = 0x102;
const Bitboard Diag_F1_H3 = 0x10204;
const Bitboard Diag_E1_H4 = 0x1020408;
const Bitboard Diag_D1_H5 = 0x102040810;
const Bitboard Diag_C1_H6 = 0x10204081020;
const Bitboard Diag_B1_H7 = 0x1020408102040;
const Bitboard Diag_A1_H8 = 0x102040810204080;
const Bitboard Diag_A2_G8 = 0x204081020408000;
const Bitboard Diag_A3_F8 = 0x408102040800000;
const Bitboard Diag_A4_E8 = 0x810204080000000;
const Bitboard Diag_A5_D8 = 0x1020408000000000;
const Bitboard Diag_A6_C8 = 0x2040800000000000;
const Bitboard Diag_A7_B8 = 0x4080000000000000;
const Bitboard Diag_A8_A8 = 0x8000000000000000;
const Bitboard DIAGS[64] = {  // An array mapping squares to their diagonals
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
const Bitboard Diag_A1_A1 = 0x80;
const Bitboard Diag_B1_A2 = 0x8040;
const Bitboard Diag_C1_A3 = 0x804020;
const Bitboard Diag_D1_A4 = 0x80402010;
const Bitboard Diag_E1_A5 = 0x8040201008;
const Bitboard Diag_F1_A6 = 0x804020100804;
const Bitboard Diag_G1_A7 = 0x80402010080402;
const Bitboard Diag_H1_A8 = 0x8040201008040201;
const Bitboard Diag_H2_B8 = 0x4020100804020100;
const Bitboard Diag_H3_C8 = 0x2010080402010000;
const Bitboard Diag_H4_D8 = 0x1008040201000000;
const Bitboard Diag_H5_E8 = 0x804020100000000;
const Bitboard Diag_H6_F8 = 0x402010000000000;
const Bitboard Diag_H7_G8 = 0x201000000000000;
const Bitboard Diag_H8_H8 = 0x100000000000000;
const Bitboard ANTI_DIAGS[64] = {  // An array mapping squares to their anti-diagonals 
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
const Bitboard SquareE1 = 8;
const Bitboard SquareE8 = 0x800000000000000;
const Bitboard SquareH1 = 1;
const Bitboard SquareF1 = 4;
const Bitboard SquareG1 = 2;
const Bitboard SquareG8 = 0x200000000000000; 
const Bitboard SquareH8 = 0x100000000000000;
const Bitboard SquareF8 = 0x400000000000000;
const Bitboard SquareA1 = 0x80;
const Bitboard SquareD1 = 0x10;
const Bitboard SquareA8 = 0x8000000000000000;
const Bitboard SquareD8 = 0x1000000000000000;
const Bitboard SquareC1 = 0x20;
const Bitboard SquareC8 = 0x2000000000000000;

// Castling indicators (see Utils.h)
const uint8_t W_Kingside  = 0x1;
const uint8_t W_Queenside = 0x2;
const uint8_t B_Kingside  = 0x4;
const uint8_t B_Queenside = 0x8;
const uint8_t No_Castle   = 0x0;

// White and black indicators (see Game.hpp)
const bool WHITE = true;
const bool BLACK = false;

// Piece indicators (see Game.hpp)
enum Piece : uint8_t {
  White_Pawn, White_Knight, White_Bishop, White_Rook, White_Queen, White_King,
  Black_Pawn, Black_Knight, Black_Bishop, Black_Rook, Black_Queen, Black_King, 
  No_Piece
};

// A map of characters to the pieces they represent
const unordered_map<char, Piece> Char_To_Piece {
  {'P', White_Pawn}, {'N', White_Knight}, {'B', White_Bishop}, {'R', White_Rook}, {'Q', White_Queen}, {'K', White_King},
  {'p', Black_Pawn}, {'n', Black_Knight}, {'b', Black_Bishop}, {'r', Black_Rook}, {'q', Black_Queen}, {'k', Black_King}
};

// State indicators (see Game.hpp)
enum State : int {
  White_Win = 1, Black_Win = -1, Draw = 0, Ongoing = 2
};

// Null indicator (see Game.h)
const Bitboard NONE = AllBits;

// Maximum ply until the game is a draw
const uint8_t MAX_PLY = 100;

// Mapping squares to their indexes
enum : Index {
  h1, g1, f1, e1, d1, c1, b1, a1,
  h2, g2, f2, e2, d2, c2, b2, a2,
  h3, g3, f3, e3, d3, c3, b3, a3,
  h4, g4, f4, e4, d4, c4, b4, a4,
  h5, g5, f5, e5, d5, c5, b5, a5,
  h6, g6, f6, e6, d6, c6, b6, a6,
  h7, g7, f7, e7, d7, c7, b7, a7,
  h8, g8, f8, e8, d8, c8, b8, a8,
  null_sq
};

// Mapping squares names to their indexes
const unordered_map<string, Bitboard> Square_To_Index = {
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

// Mapping a piece to it's ascii character representation
// We use the following convention:
// ---------------------------------
// - White Pawns   = "P"  |  -  Black Pawns   = "p"
// - White Knights = "N"  |  -  Black Knights = "n"
// - White Bishops = "B"  |  -  Black Bishops = "b"
// - White Rooks   = "R"  |  -  Black Rooks   = "r"
// - White Queens  = "Q"  |  -  Black Queens  = "q"
// - White Kings   = "K"  |  -  Black Kings   = "k"
const char Piece_To_Char[12] = {
  'P', 'N', 'B', 'R', 'Q', 'K',
  'p', 'n', 'b', 'r', 'q', 'k'
};

// A map of characters like 'P' to their location within the Boards array
const unordered_map<char, Bitboard> Char_To_Index = {
  {'K', 0}, {'k', 1}, {'Q', 2}, {'q', 3}, {'B',  4}, {'b',  5},
  {'R', 6}, {'r', 7}, {'N', 8}, {'n', 9}, {'P', 10}, {'p', 11}
};

// The character used to represent an empty square
const char EMPTY_SQ = 'X';

// Chess fen strings
const string STARTING_FEN    = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const string TRICKY_POSITION = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
#endif
