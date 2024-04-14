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

// This file is to be included at the bottom of "MoveGen.hpp", this is because
// it depends on functions and classes from both "MoveGen.hpp" and "Game.hpp",
// however these functions don't have anything to do with move generation and 
// therefore don't belong in the "MoveGen.hpp" file, furthermore these functions
// are also not necessary in the "Game.hpp" file so they are to live inside of 
// this "helper" file.

// Function to check whether any side has checkmated the other
uint8_t Game::Check_Win()
{
  // Get check mask of the side to move
  const i64& Check = (Status.Side == WHITE ? Status.White_Check : Status.Black_Check);
  
  // If there currently is a check and we have no legal moves there must be check mate
  if (Check != NONE && Generate_Moves(*this).size() == 0)
  {
    if (Status.Side == WHITE) return -1;
    if (Status.Side == BLACK) return  1;
  }
  
  return 2; // Otherwise game is still in play
}

// Function to check whether the game is a draw
uint8_t Game::Check_Draw()
{
  // 50 move-rule draws
  if (Status.Ply >= MAX_PLY) return 0;

  return 2; // Otherwise game is still in play
}

// Function to get the piece on a given square, if the square is empty 
// it will return 'X'.
char Game::Piece_On(const i64& Square)
{ 
  // If we know there is not piece on the board may return early
  if (Board.All_Pieces & Index_To_Bitboard(Square) == NoBits) return EMPTY_SQ;

  const i64* All_Boards[12] = {
    &Board.White_Pawn, &Board.White_Knight, &Board.White_Bishop,
    &Board.White_Rook, &Board.White_Queen,  &Board.White_King,
    &Board.Black_Pawn, &Board.Black_Knight, &Board.Black_Bishop,
    &Board.Black_Rook, &Board.Black_Queen,  &Board.Black_King
  };
  
  // Loop over all boards and if the bit is set return the 
  // relevant character to represent the piece
  for (int i = 0; i < 12; i++)
    if (Get_Bit(*All_Boards[i], Square)) return Index_To_Char[i];

  return EMPTY_SQ;
}

// Function to print the board
void Game::Show_Board() 
{  
  cout.tie(nullptr); // Faster output

  int rank = 8;

  cout << "\n" << rank << " | ";
  
  // Loop over all squares and print the piece on this square
  for (i64 sq = a8; sq != h1; sq--)
  {
    cout << (Piece_On(sq) != EMPTY_SQ ? Piece_On(sq) : '.') << " "; 

    // If this the end of a rank print a new line
    if (sq % 8 == 0) cout << "\n" << --rank << " | ";
  }

  // Print the last square
  if (Get_Bit(Board.All_Pieces, h1)) cout << Piece_On(h1) << " \n"; 
  else                               cout << ". \n";

  // Print letters
  cout << "    _______________\n";
  cout << "    a b c d e f g h\n\n";
}

// Function to get the FEN of the game used for debugging
string Game::Get_Fen()
{
  string fen = "";
  char   piece;
  short  empty_count = 0; // How many empty squares have we encountered in a row
  
  // Piece placement data
  // ---------------------
  // Loop over all squares .. 
  for (int sq = a8; sq >= h1; sq--)
  {
    piece = Piece_On(sq);

    // If the square is empty increment the empty count
    if (piece == EMPTY_SQ) empty_count++;

    // Otherwise if the square is not empty then add the piece and reset the empty count
    else
    { 
      if (empty_count > 0) fen += to_string(empty_count);
      fen        += piece;
      empty_count = 0;
    }
    
    // If this is the end of a rank then add a slash
    if (sq % 8 == 0)
    {
      if (empty_count > 0) fen += to_string(empty_count);
      if (sq != h1)        fen += "/";
      empty_count = 0;
    }
  }

  // Active colour
  // -------------_
  fen += (Status.Side == WHITE ? " w " : " b ");

  // Castling availability
  // ----------------------
  if (Status.Castle_Rights == No_Castle) fen += "-";
  else
  {
    if (Status.Castle_Rights & W_Kingside)  fen += "K";
    if (Status.Castle_Rights & W_Queenside) fen += "Q";
    if (Status.Castle_Rights & B_Kingside)  fen += "k";
    if (Status.Castle_Rights & B_Queenside) fen += "q";
  }

  // En passant square
  // ------------------
  if (Status.En_Passant == NONE) fen += " - ";
  else                           fen += " " + Index_To_Square[Get_Index(Status.En_Passant)] + " ";

  // Halfmove and fullmove
  // ----------------------
  fen += to_string(Status.Ply) + " " + to_string(Status.Fullmove);
  
  return fen;
}

// Function to check if a square index is attacked by a given colour
// We use the square location and assume that at this square is a pawn, knight, bishop ... etc.
// then we look at the squares that it can attack and if a pawn, knight, bishop ... etc.
// of the enemy colour exists on this square then, this square is attacked.
//
// Better explanation can be found here:
// https://www.chessprogramming.org/Square_Attacked_By#:~:text=determines%20whether%20a%20square%20is,generate%20all%20pseudo%20legal%20moves.
bool Game::Is_Square_Attacked(const i64& Square, const bool& Colour, const i64& Remove_sq) const
{
  // Does a pawn attack this square?
  const array<i64, 64>& Pawn_Attacks = (Colour == WHITE ? BLACK_PAWN_ATKS  : WHITE_PAWN_ATKS); 
  const i64& Enemy_Pawns             = (Colour == WHITE ? Board.White_Pawn : Board.Black_Pawn);

  if (Pawn_Attacks[Square] & Enemy_Pawns) return true;

  // Does a knight attack this square?
  const i64& Enemy_Knights = (Colour == WHITE ? Board.White_Knight : Board.Black_Knight);

  if (KNIGHT_MOVES[Square] & Enemy_Knights) return true;

  // We need to remove the 1 bit of this square from the occupancy (if it exists)
  // otherwise hyperbola quintessence will generate moves incorrectly
  i64 Occupancy                    = Remove_Bit(Board.All_Pieces, Square); 
  if (Remove_sq != NONE) Occupancy = Remove_Bit(Occupancy, Remove_sq); // See (MoveGen.hpp - line 195)
   
  // Does a bishop or a queen attack this square?
  const i64 Enemy_BishopQueen = (Colour == WHITE ? Board.White_Bishop | Board.White_Queen : 
                                                   Board.Black_Bishop | Board.Black_Queen);

  if (Compute_Bishop_attacks(Index_To_Bitboard(Square), Occupancy) & Enemy_BishopQueen) 
    return true;

  // Does a rook or a queen attack this square?
  const i64 Enemy_RookQueen = (Colour == WHITE ? Board.White_Rook | Board.White_Queen :
                                                 Board.Black_Rook | Board.Black_Queen);

  if (Compute_Rook_attacks(Index_To_Bitboard(Square), Occupancy) & Enemy_RookQueen) 
    return true;

  return false; // If none of the above are true then the square is not attacked
}
