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
#ifndef GAME_HPP
#define GAME_HPP
#include <sstream>
#include <cstdlib>
#include <ctype.h>
#include "Utils.hpp"

using namespace std;
typedef unsigned long long i64;
// This structure will organise all information about a move
// ----------------------------------------------------------
// We will use the following convention for piece promotions:
// - 0 = No promotion
// - 1 = Knight
// - 2 = Bishop
// - 3 = Rook
// - 4 = Queen
//
// We will use the following convention for piece indicators:
// - 0 = Pawn
// - 1 = Knight
// - 2 = Bishop
// - 3 = Rook
// - 4 = Queen
// - 5 = King
//
// We will use a flag of all bits set to 1 to represent a null move,
// a move that is nothing used as the "Last_Move" attribute of a game
// class once it has just been initialised
struct Move
{
  // Attributes
  i64  From;
  i64  To;
  i64  Piece;
  bool Capture;
  i64  Promoted_Piece;
  bool En_Passant;  

  // Constructor to initialise a move
  Move(i64 From_, i64 To_, i64 Piece_, bool Capture_, i64 Promoted_Piece_, bool En_Passant_)
  {
    From           = From_;
    To             = To_;
    Piece          = Piece_;
    Capture        = Capture_;
    Promoted_Piece = Promoted_Piece_;
    En_Passant     = En_Passant_; 
  }
};

// This structure will organise all information about a game's status
// -------------------------------------------------------------------
// - Side: White is "true" , Black is "false" 
//
// - En_Passant: Location of the en passant square (will be set to AllBits if no en passant square)
//
// - Ply: Halfmove clock, increments after any player's move
//
// - Fullmove: Fullmove clock, increments after black's move starts at 1
//
// - Castle_Rights: Castling right of both players, we use the following convention
//   --------------------------------------------
//   BINARY  -> RIGHTS
//   --------------------------------------------
//   0 0 0 0 -> No rights
//   1 0 0 0 -> White has queenside castle rights
//   0 1 0 0 -> White has kingside castle rights
//   0 0 1 0 -> Black has queenside castle rights
//   0 0 0 1 -> Black has kingside castle rights
//
// - Status: 1 is white has won, 0 means draw, -1 means black has won and 2 means game is ongoing
//
// - Pins: A map of bitboards to the attack rays they may move along if they are pinned
//
// - White_Check: A mask containing the attack ray of any checks on the white king (will be set to AllBits if no checks)
//
// - Black_Check: A mask containing the attack ray of any checks on the black king (will be set to AllBits if no checks)
//
// - Last_Move: The last move played onto the board
struct Game_Status
{
  // Attributes
  bool    Side;
  i64     En_Passant;
  i64     Ply;
  i64     Fullmove;
  uint8_t Castle_Rights;
  uint8_t Status;
  unordered_map<i64, i64> Pins;
  i64 White_Check = AllBits; // We assume there is no check by default
  i64 Black_Check = AllBits; // We assume there is no check by default
  Move Last_Move  = Move(NONE, NONE, NONE, false, NONE, false);

  // Constructor to initialise a game status
  void Init_Game_Status(bool Side_, i64 En_Passant_, i64 Ply_, i64 Fullmove_, uint8_t Castle_Rights_,
                        uint8_t Status_)
  {
    Side          = Side_;
    En_Passant    = En_Passant_;
    Ply           = Ply_;
    Fullmove      = Fullmove_;
    Castle_Rights = Castle_Rights_;
    Status        = Status_;
  }
};

// This structure will organise all information about a board's status
// --------------------------------------------------------------------
// Each attribute is a bitboard representing the location of the relevant pieces
struct Board_Status
{
  // Attributes
  i64 White_King;      i64 Black_King;
  i64 White_Queen;     i64 Black_Queen;
  i64 White_Bishop;    i64 Black_Bishop;
  i64 White_Rook;      i64 Black_Rook;
  i64 White_Knight;    i64 Black_Knight;
  i64 White_Pawn;      i64 Black_Pawn;
  i64 White_All;       i64 Black_All;

  i64 All_Pieces;
  
  // Constructor to initialise a board status
  void Init_Board_Status(i64 White_King_, i64 Black_King_, i64 White_Queen_, i64 Black_Queen_,
                         i64 White_Bishop_, i64 Black_Bishop_, i64 White_Rook_, i64 Black_Rook_,
                         i64 White_Knight_, i64 Black_Knight_, i64 White_Pawn_, i64 Black_Pawn_){
    White_King   = White_King_;          Black_King   = Black_King_;
    White_Queen  = White_Queen_;         Black_Queen  = Black_Queen_;
    White_Bishop = White_Bishop_;        Black_Bishop = Black_Bishop_;
    White_Rook   = White_Rook_;          Black_Rook   = Black_Rook_;
    White_Knight = White_Knight_;        Black_Knight = Black_Knight_;
    White_Pawn   = White_Pawn_;          Black_Pawn   = Black_Pawn_;

    White_All    = White_King | White_Queen | White_Bishop | White_Rook | White_Knight | White_Pawn;
    Black_All    = Black_King | Black_Queen | Black_Bishop | Black_Rook | Black_Knight | Black_Pawn;
    All_Pieces   = White_All  | Black_All;
  }
};

// Game utility functions
// -----------------------
// The following is a collection of functions that are needed in the game class
//
// Function to check if a square index is attacked by a given colour
bool Is_Square_Attacked(Board_Status Boards, i64 Square, bool Colour)
{
  // We use the square location and assume that at this square is a pawn, knight, bishop ... etc.
  // then we look at the squares that it can attack and if a pawn, knight, bishop ... etc.
  // of the enemy colour exists on this square then, this square is attacked
  
  // Enemy pieces
  i64 Enemy_Pawns   = (Colour == WHITE ? Boards.White_Pawn   : Boards.Black_Pawn);
  i64 Enemy_Knights = (Colour == WHITE ? Boards.White_Knight : Boards.Black_Knight);
  i64 Enemy_Bishops = (Colour == WHITE ? Boards.White_Bishop : Boards.Black_Bishop);
  i64 Enemy_Rooks   = (Colour == WHITE ? Boards.White_Rook   : Boards.Black_Rook);
  i64 Enemy_Queens  = (Colour == WHITE ? Boards.White_Queen  : Boards.Black_Queen);

  const array<i64, 64> Pawn_Attacks = (Colour == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS); 

  // Pawn attacks
  if (Pawn_Attacks[Square] & Enemy_Pawns) return true;
  // Knight attacks
  if (KNIGHT_MOVES[Square] & Enemy_Knights) return true;

  // We need to remove the 1 bit from the occupancy (if it exists)
  // otherwise hyperbola quintessence will generate moves incorrectly
  i64 Occupancy = Remove_Bit(Boards.All_Pieces, Square); 
  
  // Bishop attacks
  if (Compute_Bishop_attacks(Index_To_Bitboard(Square), Occupancy) & Enemy_Bishops) return true;
  // Rook attacks
  if (Compute_Rook_attacks(Index_To_Bitboard(Square), Occupancy) & Enemy_Rooks) return true;
  // Queen attacks
  if (Compute_Queen_attacks(Index_To_Bitboard(Square), Occupancy) & Enemy_Queens) return true;
  
  return false; // If none of the above are true then the square is not attacked
}

// Function to get the check mask for a given colour, we define the 
// check mask to be the attacking ray from the checker to the king,
// if an attack was made by a non-slider piece the check mask is simply
// the location of the checking piece, this function follows code similar to 
// the Is_Square_Attacked function explained above
i64 Get_Check_Mask(Board_Status Boards, bool Colour)
{
  i64 King_Location = (Colour == WHITE ? Boards.White_King : Boards.Black_King);
  i64 King_Index    = Get_Index(King_Location);

  // Enemy pieces
  i64 Enemy_Pawns   = (Colour == WHITE ? Boards.Black_Pawn   : Boards.White_Pawn);
  i64 Enemy_Knights = (Colour == WHITE ? Boards.Black_Knight : Boards.White_Knight);
  i64 Enemy_Bishops = (Colour == WHITE ? Boards.Black_Bishop : Boards.White_Bishop);
  i64 Enemy_Rooks   = (Colour == WHITE ? Boards.Black_Rook   : Boards.White_Rook);
  i64 Enemy_Queens  = (Colour == WHITE ? Boards.Black_Queen  : Boards.White_Queen);

  const array<i64, 64> Pawn_Attacks = (Colour == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS); 

  // Non-slider pieces
  if (Pawn_Attacks[King_Index] & Enemy_Pawns)   return Pawn_Attacks[King_Index] & Enemy_Pawns;
  if (KNIGHT_MOVES[King_Index] & Enemy_Knights) return KNIGHT_MOVES[King_Index] & Enemy_Knights;

  // We need to remove the 1 bit from the occupancy (if it exists)
  // otherwise hyperbola quintessence will generate moves incorrectly
  i64 Occupancy = Remove_Bit(Boards.All_Pieces, King_Index);

  // Slider pieces
  if (Compute_Bishop_attacks(King_Location, Occupancy) & Enemy_Bishops)
  {
    i64 Attacker = Compute_Bishop_attacks(King_Location, Occupancy) & Enemy_Bishops;
    return Remove_Bit(Create_Ray(King_Location, Attacker), King_Index); // Can't move onto the king
  }
  if (Compute_Rook_attacks(King_Location, Occupancy) & Enemy_Rooks)
  {
    i64 Attacker = Compute_Rook_attacks(King_Location, Occupancy) & Enemy_Rooks;
    return Remove_Bit(Create_Ray(King_Location, Attacker), King_Index);
  }
  if (Compute_Queen_attacks(King_Location, Occupancy) & Enemy_Queens)
  {
    i64 Attacker = Compute_Queen_attacks(King_Location, Occupancy) & Enemy_Queens;
    return Remove_Bit(Create_Ray(King_Location, Attacker), King_Index);
  }

  return AllBits; // If there is no check then our check maskcan be all bits as 1
}

// This class will organise all information needed to play a game of chess with this engine
// -----------------------------------------------------------------------------------------
class Game
{
  public:
    Board_Status Board;
    Game_Status Status;
    
    // Constructor to initialise a game from a fen string
    Game(string Fen) 
    {
      string Piece_Locations, Colour, Castle_Rights, En_Passant, Halfmove, Fullmove;

      // Separates fen into 6 strings each containing the information above 
      stringstream splitter(Fen);
      splitter >> Piece_Locations >> Colour >> Castle_Rights >> En_Passant >> Halfmove >> Fullmove;

      // Initialising piece locations
      // -----------------------------
      i64 Square     = 63;
      i64 Boards[12] = {};
      // A map of characters like 'P' to their location within the Boards array
      static unordered_map<char, i64> Char_To_Index = {
        {'K', 0}, {'k', 1}, {'Q', 2}, {'q', 3}, {'B', 4},  {'b', 5},
        {'R', 6}, {'r', 7}, {'N', 8}, {'n', 9}, {'P', 10}, {'p', 11}
      };
      
      // Iterate over all characters ..
      for (char piece : Piece_Locations)
      {
        // If the character is '/' we move to the next row, and if the character
        // is a number we skip that many squares, otherwise we add the piece to the relevant bitboard
        if (piece == '/') continue;
        else if (isdigit(piece)) Square -= stoi(&piece); 
        else 
        {
          Boards[Char_To_Index[piece]] = Set_Bit(Boards[Char_To_Index[piece]], Square); 
          Square--;
        }
      }

      Board.Init_Board_Status(Boards[0], Boards[1], Boards[2], Boards[3], Boards[4], Boards[5],
                              Boards[6], Boards[7], Boards[8], Boards[9], Boards[10], Boards[11]);

      // Initialising game status variables
      // -----------------------------------
      bool side             = (Colour == "w");
      i64 en_passant        = (En_Passant != "-" ? Square_To_Index[En_Passant] : AllBits);
      i64 ply               = stoi(Halfmove);
      i64 fullmove          = stoi(Fullmove);
      uint8_t castle_rights = 0;
      for (char castle : Castle_Rights)
      {
        if      (castle == 'K') castle_rights |= W_Kingside;
        else if (castle == 'Q') castle_rights |= W_Queenside;
        else if (castle == 'k') castle_rights |= B_Kingside;
        else if (castle == 'q') castle_rights |= B_Queenside;
      }
      uint8_t status = 2; // We assume that the game is ongoing

      Status.Init_Game_Status(side, en_passant, ply, fullmove, castle_rights, status);
      Update(); // Ensure that all pins and check masks have been found
    }
    
    // Function to play a move onto the board
    void Make_Move(Move M)
    {
      i64 From_Index = Get_Index(M.From);
      i64 To_Index   = Get_Index(M.To);

      // Map of piece codes to the pointers of their relevant bitboards (see the top of this file)
      i64* W_Piece_To_Bitboard[6] = { 
         &Board.White_Pawn, &Board.White_Knight, &Board.White_Bishop,
         &Board.White_Rook, &Board.White_Queen,  &Board.White_King
      };

      i64* B_Piece_To_Bitboard[6] = {
         &Board.Black_Pawn, &Board.Black_Knight, &Board.Black_Bishop,
         &Board.Black_Rook, &Board.Black_Queen,  &Board.Black_King
      };
      
      // We need to remove the bit of the moving piece's bitboard and replace it with the new location
      if (Status.Side == WHITE && M.Promoted_Piece == NO_PROMO) // Promotions will be handled below
      {
        i64* board = W_Piece_To_Bitboard[M.Piece];
        *board = Remove_Bit(*board, From_Index);
        *board = Set_Bit(*board,    To_Index);
      }
      
      else if (M.Promoted_Piece == NO_PROMO)
      {
        i64* board = B_Piece_To_Bitboard[M.Piece]; 
        *board = Remove_Bit(*board, From_Index);
        *board = Set_Bit(*board,    To_Index);
      }

      // We should remove the bit of any captured pieces
      // We will first handle en-passant captures
      if (M.En_Passant)
      { 
        // The pawn underneath the attacking pawn is captured
        i64 Under_1_Index = Get_Index(Shift_Down(M.To, Status.Side));
        if (Status.Side == WHITE) Board.Black_Pawn = Remove_Bit(Board.Black_Pawn, Under_1_Index);
        else                      Board.White_Pawn = Remove_Bit(Board.White_Pawn, Under_1_Index);
      }

      else if (M.Capture)
      {
        // We need to find the piece that we are capturing
        if (Status.Side == WHITE)
        {
          for (i64* Board : W_Piece_To_Bitboard)
          {
            if (Get_Bit(*Board, To_Index)) {*Board = Remove_Bit(*Board, To_Index); break;}
          }
        }

        else
        {
          for (i64* Board : B_Piece_To_Bitboard) 
          {
            if (Get_Bit(*Board, To_Index)) {*Board = Remove_Bit(*Board, To_Index); break;}
          }
        }
      }

      // We need to handle promotion moves
      if (M.Promoted_Piece != NO_PROMO)
      {
        if (Status.Side == WHITE)
        {
          // Remove the old pawn
          Board.White_Pawn = Remove_Bit(Board.White_Pawn, From_Index);

          // Add the promoted piece
          i64 *Promoted_Bitboard = W_Piece_To_Bitboard[M.Promoted_Piece];
          *Promoted_Bitboard     = Set_Bit(*Promoted_Bitboard, To_Index);
        }
        else
        {
          // Remove the old pawn
          Board.Black_Pawn = Remove_Bit(Board.Black_Pawn, From_Index);

          // Add the promoted piece
          i64 *Promoted_Bitboard = B_Piece_To_Bitboard[M.Promoted_Piece];
          *Promoted_Bitboard     = Set_Bit(*Promoted_Bitboard, To_Index);
        }
      }

      // Update occupancy bitboards
      Board.White_All  = (Board.White_Pawn | Board.White_Knight | Board.White_Bishop | 
                          Board.White_Rook | Board.White_Queen  | Board.White_King); 

      Board.Black_All  = (Board.Black_Pawn | Board.Black_Knight | Board.Black_Bishop |
                          Board.Black_Rook | Board.Black_Queen  | Board.Black_King);

      Board.All_Pieces = Board.White_All | Board.Black_All;
      Status.Last_Move = M; // Update last move
      Update();
      Status.Side = ~Status.Side; // Change side
    }

    // Function to print the board
    void Show_Board() 
    {  
      // A map from indexes into the 'all pieces' array to their relevant ascii characters
      static unordered_map<short, char> Index_To_Char = {
        {0, 'P'}, {1, 'N'}, {2, 'B'}, {3, 'R'}, {4,  'Q'}, {5,  'K'},
        {6, 'p'}, {7, 'n'}, {8, 'b'}, {9, 'r'}, {10, 'q'}, {11, 'k'}
      };

      // An array of pointers to all of the piece bitboards
      i64* All_Bitboards[12] = {
        &Board.White_Pawn,   &Board.Black_Pawn,   &Board.White_Knight, &Board.Black_Knight,
        &Board.White_Bishop, &Board.Black_Bishop, &Board.White_Rook,   &Board.Black_Rook,
        &Board.White_Queen,  &Board.Black_Queen,  &Board.White_King,   &Board.Black_King
      };

      int rank = 8, count;
      cout << rank << " | ";

      // Loop over all squares
      for (i64 sq = a8; sq != h1; sq--)
      {
        // If the square is set print the relevant character
        if (Get_Bit(Board.All_Pieces, sq))
        {
          count = 0;
          for (i64* Board : All_Bitboards)
          {
            if (Get_Bit(*Board, sq)) {cout << Index_To_Char[count] << " "; break;}
            ++count;
          }
        }
        else cout << ". ";

        // If this the end of a rank print a new line
        if (sq % 8 == 0) cout << "\n" << --rank << " | ";
      }

      // Print the last square
      if (Get_Bit(Board.All_Pieces, h1))
      {
        count = 0;
        for (i64* Board : All_Bitboards)
        {
          if (Get_Bit(*Board, h1)) {cout << Index_To_Char[count] << " \n"; break;}
          ++count;
        }
      }
      else cout << ". \n";

      // Print letters
      cout << "    _______________\n";
      cout << "    a b c d e f g h\n\n";
    }
  
  private:
    // A function to find pins in the current position
    unordered_map<i64, i64> Get_Pins(bool Colour)
    {
      // To generate any pin masks we should notice that only sliding pieces
      // may pin other pieces, with this in mind consider getting the move 
      // bitboard of a queen from the square where the king is located.
      // If the only 2 pieces are the pinner then the 'pinnee' AND the pinner is 
      // in front of the 'pinnee' then we have found a pin
      // 'Pinnee': A piece that is being pinned 

      unordered_map<i64, i64> Pins;
      // Get working variables
      i64 FriendlyKing = Colour == WHITE ? Board.White_King   : Board.Black_King;
      i64 FriendlyAll  = Colour == WHITE ? Board.White_All    : Board.Black_All;
      i64 EnemyQueen   = Colour == WHITE ? Board.Black_Queen  : Board.White_Queen;
      i64 EnemyRook    = Colour == WHITE ? Board.Black_Rook   : Board.White_Rook;
      i64 EnemyBishop  = Colour == WHITE ? Board.Black_Bishop : Board.White_Bishop;
      
      // Get locations of enemy slider pieces aligned with our king these will be
      // our potential pinners 
      i64 Pinners = (
        // Enemy pieces on diagonal rays
        (Compute_Bishop_attacks(FriendlyKing, NoBits) & (EnemyQueen | EnemyBishop)) |

        // Enemy pieces on horizontal and vertical rays
        (Compute_Rook_attacks(FriendlyKing, NoBits) & (EnemyQueen | EnemyRook))
      );

      // Iterate through all potential pinners and check if a pinned piece is on this ray
      i64 Attacker, Pinnee;
      while (Pinners)
      {
        Attacker = Get_LSB(Pinners);

        // Get potential 'pinnee's, by building a ray from the attacker to the king
        Pinnee = Create_Ray(FriendlyKing, Attacker) & FriendlyAll;  
        
        // There can only be on pinned piece along a ray otherwise this wouldn't be a pin
        if (BitCount(Pinnee) == 1)
        {
          // Then the only moves that this piece may make is along this ray or to capture the attacker
          Pins[Pinnee] = Set_Bit(Create_Ray(Pinnee, Attacker), Get_Index(Attacker)); 
        }

        Pinners = Remove_Bit(Pinners, Get_Index(Attacker));
      }

      return Pins;
    }

    // A function to find checks in the current position
    void Get_Checks()
    {
      Status.White_Check = Get_Check_Mask(Board, WHITE);
      Status.Black_Check = Get_Check_Mask(Board, BLACK);
    }
    
    // A function to update the current en-passant square
    void Get_En_Passant()
    {
      // If there is no last move then we do not need to update the en-passant square
      if (Status.Last_Move.From == NONE) return;

      // If the last move was not a pawn then we can set the en-passant square to all bits
      if (Status.Last_Move.Piece != PAWN)
      {
        Status.En_Passant = AllBits;
        return;
      }

      // If the last move was a double pawn push then we can update the en passant square
      // New en-passant square will be the square underneath the pawn that was just pushed
      int Difference = Get_Index(Status.Last_Move.To) - Get_Index(Status.Last_Move.From);    
      if (abs(Difference) == 16)
      {
        // If current side is white this means that the last move was from black 
        if (Status.Side == WHITE) Status.En_Passant = Index_To_Bitboard(Get_Index(Status.Last_Move.To) + 8); 
        // Means that the last move was from white
        else                      Status.En_Passant = Index_To_Bitboard(Get_Index(Status.Last_Move.To) - 8);
      }
    }
    
    // A function to update castling rights
    void Get_Castle_Rights()
    {
      // If there is no last move or the last move wasn't from the king
      // or rook we dont need to update the castle rights
      if (Status.Last_Move.From  == NONE || 
         (Status.Last_Move.Piece != KING && Status.Last_Move.Piece != ROOK)) return;

      int From_Index = Get_Index(Status.Last_Move.From);
      int To_Index   = Get_Index(Status.Last_Move.To);
      
      // If the last move was a rook moving for the first time we remove
      // castle rights for that side
      if (Status.Last_Move.Piece == ROOK)
      {
        if (Status.Side == WHITE)
        {
          if (From_Index == h1) Status.Castle_Rights &= ~W_Kingside;
          if (From_Index == a1) Status.Castle_Rights &= ~W_Queenside;
        }
        else
        {
          if (From_Index == h8) Status.Castle_Rights &= ~B_Kingside;
          if (From_Index == a8) Status.Castle_Rights &= ~B_Queenside;
        }
        return;
      }

      // If the last move was by a king that didn't castle we remove all rights
      if (abs(From_Index - To_Index) != 2) 
      {
        if (Status.Side == WHITE) Status.Castle_Rights &= ~(W_Queenside | W_Kingside);
        if (Status.Side == BLACK) Status.Castle_Rights &= ~(B_Queenside | B_Kingside);
        return;
      }

      // If last move was a castle we need to ensure that we have moved the rook
      if (Status.Side == WHITE)
      {
        if (To_Index == g1)
        {
          // Kingside castle
          Board.White_Rook = Remove_Bit(Board.White_Rook, h1);
          Board.White_Rook = Set_Bit(Board.White_Rook, f1);
        }
        if (To_Index == c1)
        {
          // Queenside castle
          Board.White_Rook = Remove_Bit(Board.White_Rook, a1);
          Board.White_Rook = Set_Bit(Board.White_Rook, d1);
        }
        Status.Castle_Rights &= ~(W_Queenside | W_Kingside); // We remove all rights after castling
      }
      else
      {
        if (To_Index == g8)
        {
          // Kingside castle
          Board.Black_Rook = Remove_Bit(Board.Black_Rook, h8);
          Board.Black_Rook = Set_Bit(Board.Black_Rook, f8);
        }
        if (To_Index == c8)
        {
          // Queenside castle
          Board.Black_Rook = Remove_Bit(Board.Black_Rook, a8);
          Board.Black_Rook = Set_Bit(Board.Black_Rook, d8);
        }
        Status.Castle_Rights &= ~(B_Queenside | B_Kingside); // We remove all rights after castling
      }
    }

    // A function to update the game's status, this includes updating the en passant square
    // getting new pins if any, and getting new checks if any were found and more
    void Update()
    {
      Get_Checks(); 
      Status.Pins = Get_Pins(WHITE);
      Status.Pins.insert(Get_Pins(BLACK).begin(), Get_Pins(BLACK).end());
      Get_En_Passant();
      Get_Castle_Rights();
    }
};
#endif
