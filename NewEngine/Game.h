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
#include <unordered_map>
#include <string>
#include <sstream>
#include <cstdint>
#include <ctype.h>
#include "Utils.h"

#include <iostream>
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
struct Move{
  // Attributes
  i64 From;
  i64 To;
  i64 Piece;
  bool Capture;
  i64 Promoted_Piece;
  bool En_Passant;  

  // Constructor to initialise a move
  Move(i64 From_, i64 To_, i64 Piece_, bool Capture_, i64 Promoted_Piece_, bool En_Passant_){
    From = From_;
    To = To_;
    Piece = Piece_;
    Capture = Capture_;
    Promoted_Piece = Promoted_Piece_;
    En_Passant = En_Passant_; 
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
//   ------------------
//   BINARY  -> RIGHTS
//   ------------------
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
// - White_Check: A mask containing the attack ray of any checks on the white king
//
// - Black_Check: A mask containing the attack ray of any checks on the black king
struct Game_Status{
  // Attributes
  bool Side;
  i64 En_Passant;
  i64 Ply;
  i64 Fullmove;
  uint8_t Castle_Rights;
  uint8_t Status;
  unordered_map<i64, i64> Pins;
  i64 White_Check;
  i64 Black_Check;

  // Constructor to initialise a game status
  void Init_Game_Status(bool Side_, i64 En_Passant_, i64 Ply_, i64 Fullmove_, uint8_t Castle_Rights_,
                        uint8_t Status_){
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
struct Board_Status{
  // Attributes
  i64 White_King;
  i64 Black_King;
  i64 White_Queen;
  i64 Black_Queen;
  i64 White_Bishop;
  i64 Black_Bishop;
  i64 White_Rook;
  i64 Black_Rook;
  i64 White_Knight;
  i64 Black_Knight;
  i64 White_Pawn;
  i64 Black_Pawn; 
  i64 White_All;
  i64 Black_All;
  i64 All_Pieces;
  
  // Constructor to initialise a board status
  void Init_Board_Status(i64 White_King_, i64 Black_King_, i64 White_Queen_, i64 Black_Queen_,
                         i64 White_Bishop_, i64 Black_Bishop_, i64 White_Rook_, i64 Black_Rook_,
                         i64 White_Knight_, i64 Black_Knight_, i64 White_Pawn_, i64 Black_Pawn_){
    White_King   = White_King_;
    Black_King   = Black_King_;
    White_Queen  = White_Queen_;
    Black_Queen  = Black_Queen_;
    White_Bishop = White_Bishop_;
    Black_Bishop = Black_Bishop_;
    White_Rook   = White_Rook_;
    Black_Rook   = Black_Rook_;
    White_Knight = White_Knight_;
    Black_Knight = Black_Knight_;
    White_Pawn   = White_Pawn_;
    Black_Pawn   = Black_Pawn_;
    White_All    = White_King | White_Queen | White_Bishop | White_Rook | White_Knight | White_Pawn;
    Black_All    = Black_King | Black_Queen | Black_Bishop | Black_Rook | Black_Knight | Black_Pawn;
    All_Pieces   = White_All | Black_All;
  }
};

// Game utility functions
// -----------------------
// The following is a collection of functions that are needed in the game class
//
// Function to check if a square index is attacked by a given colour
bool Is_Square_Attacked(Board_Status Boards, i64 Square, bool Colour){
  // We use the square location and assume that at this square is a pawn, knight, bishop ... etc.
  // then we look at the squares that it can attack and if a pawn, knight, bishop ... etc.
  // of the enemy colour exists on this square then, this square is attacked
  
  // Enemy pieces
  i64 Enemy_Pawns   = Colour == WHITE ? Boards.White_Pawn   : Boards.Black_Pawn;
  i64 Enemy_Knights = Colour == WHITE ? Boards.White_Knight : Boards.Black_Knight;
  i64 Enemy_Bishops = Colour == WHITE ? Boards.White_Bishop : Boards.Black_Bishop;
  i64 Enemy_Rooks   = Colour == WHITE ? Boards.White_Rook   : Boards.Black_Rook;
  i64 Enemy_Queens  = Colour == WHITE ? Boards.White_Queen  : Boards.Black_Queen;

  const i64 (&Pawn_Attacks)[64] = Colour == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS; 

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

// Function to make a horizontal or diagonal ray from one square to another
// NOTE: You must first check the horizontal case, reason being: consider
// 2 queens of opposite colour 1 square away from each other, if you
// check the diagonal case first you will get an incorrect ray.
i64 Create_Ray(i64 From, i64 To){
  // Handle edge cases
  if (From == To || From == NoBits || To == NoBits) return 0;

  // Make ray horizontally
  if ((Compute_Rook_attacks(From, NoBits) & Compute_Rook_attacks(To, NoBits)) != 0){
    return Compute_Rook_attacks(From, NoBits) & Compute_Rook_attacks(To, NoBits);
  }
  // Make ray diagonally
  if ((Compute_Bishop_attacks(From, NoBits) & Compute_Bishop_attacks(To, NoBits)) != 0){
    return Compute_Bishop_attacks(From, NoBits) & Compute_Bishop_attacks(To, NoBits);
  }
  return 0;
}

// Function to get the check mask for a given colour, we define the 
// check mask to be the attacking ray from the checker to the king,
// if an attack was made by a non-slider piece the check mask is simply
// the location of the checking piece, this function follows code similar to 
// the Is_Square_Attacked function explained above
i64 Get_Check_Mask(Board_Status Boards, bool Colour){
  i64 KingLocation = Colour == WHITE ? Boards.White_King : Boards.Black_King;

  // Enemy pieces
  i64 Enemy_Pawns   = Colour == WHITE ? Boards.Black_Pawn   : Boards.White_Pawn;
  i64 Enemy_Knights = Colour == WHITE ? Boards.Black_Knight : Boards.White_Knight;
  i64 Enemy_Bishops = Colour == WHITE ? Boards.Black_Bishop : Boards.White_Bishop;
  i64 Enemy_Rooks   = Colour == WHITE ? Boards.Black_Rook   : Boards.White_Rook;
  i64 Enemy_Queens  = Colour == WHITE ? Boards.Black_Queen  : Boards.White_Queen;

  const i64 (&Pawn_Attacks)[64] = Colour == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS; 

  // Non-slider pieces
  if (Pawn_Attacks[KingLocation] & Enemy_Pawns) return Pawn_Attacks[KingLocation] & Enemy_Pawns;
  if (KNIGHT_MOVES[KingLocation] & Enemy_Knights) return KNIGHT_MOVES[KingLocation] & Enemy_Knights;

  // We need to remove the 1 bit from the occupancy (if it exists)
  // otherwise hyperbola quintessence will generate moves incorrectly
  i64 Occupancy = Remove_Bit(Boards.All_Pieces, KingLocation);

  // Slider pieces
  if (Compute_Bishop_attacks(KingLocation, Occupancy) & Enemy_Bishops){
    i64 Attacker = Compute_Bishop_attacks(KingLocation, Occupancy) & Enemy_Bishops;
    return Create_Ray(KingLocation, Attacker);
  }
  if (Compute_Rook_attacks(KingLocation, Occupancy) & Enemy_Rooks){
    i64 Attacker = Compute_Rook_attacks(KingLocation, Occupancy) & Enemy_Rooks;
    return Create_Ray(KingLocation, Attacker);
  }
  if (Compute_Queen_attacks(KingLocation, Occupancy) & Enemy_Queens){
    i64 Attacker = Compute_Queen_attacks(KingLocation, Occupancy) & Enemy_Queens;
    return Create_Ray(KingLocation, Attacker);
  }
  return AllBits; // If there is no check then our check maskcan be all bits as 1
}

// This class will organise all information needed to play a game of chess with this engine
// -----------------------------------------------------------------------------------------
class Game{
  public:
    Board_Status Board;
    Game_Status Status;
    
    // Constructor to initialise a game from a fen string
    Game(string Fen){
      string Piece_Locations, Colour, Castle_Rights, En_Passant, Halfmove, Fullmove;

      // Separates fen into 6 strings each containing the information above 
      stringstream splitter(Fen);
      splitter >> Piece_Locations >> Colour >> Castle_Rights >> En_Passant >> Halfmove >> Fullmove;

      // Initialising piece locations
      // -----------------------------
      i64 Square = 63;
      i64 Boards[12] = {};
      // A map of characters like 'P' to their location within the Boards array
      unordered_map<char, i64> Char_To_Index = {
        {'K', 0}, {'k', 1}, {'Q', 2}, {'q', 3}, {'B', 4}, {'b', 5},
        {'R', 6}, {'r', 7}, {'N', 8}, {'n', 9}, {'P', 10}, {'p', 11}
      };
      
      // Iterate over all characters ..
      for (char piece : Piece_Locations){
        // If the character is '/' we move to the next row, and if the character
        // is a number we skip that many squares, otherwise we add the piece to the relevant bitboard
        if (piece == '/') continue;
        else if (isdigit(piece)) Square -= stoi(&piece); 
        else {
          Boards[Char_To_Index[piece]] = Set_Bit(Boards[Char_To_Index[piece]], Square); 
          Square--;
        }
      }

      Board.Init_Board_Status(Boards[0], Boards[1], Boards[2], Boards[3], Boards[4], Boards[5],
                              Boards[6], Boards[7], Boards[8], Boards[9], Boards[10], Boards[11]);

      // Initialising game status variables
      // -----------------------------------
      bool side = Colour == "w";
      i64 en_passant = En_Passant != "-" ? Square_To_Index[En_Passant] : AllBits;
      i64 ply = stoi(Halfmove);
      i64 fullmove = stoi(Fullmove);
      uint8_t castle_rights = 0;
      for (char castle : Castle_Rights){
        if (castle == 'K') castle_rights |= White_Kingside;
        else if (castle == 'Q') castle_rights |= White_Queenside;
        else if (castle == 'k') castle_rights |= Black_Kingside;
        else if (castle == 'q') castle_rights |= Black_Queenside;
      }
      uint8_t status = 2; // We assume that the game is ongoing

      Status.Init_Game_Status(side, en_passant, ply, fullmove, castle_rights, status);
      Update(); // Ensure that all pins and check masks have been found
    }
  
  private:
    // A function to find pins in the current position
    unordered_map<i64, i64> Get_Pins(bool Colour){
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
      while (Pinners){
        Attacker = Get_LSB(Pinners);

        // Get potential 'pinnee's, by building a ray from the attacker to the king
        Pinnee = Create_Ray(FriendlyKing, Attacker) & FriendlyAll;  
        
        // There can only be on pinned piece along a ray otherwise this wouldn't be a pin
        if (BitCount(Pinnee) == 1){

          // Then the only moves that this piece may make is along this ray or to capture the attacker
          Pins[Pinnee] = Set_Bit(Create_Ray(Pinnee, Attacker), Get_Index(Attacker)); 
        }
        Pinners = Remove_Bit(Pinners, Get_Index(Attacker));
      }
      return Pins;
    }

    // A function to find checks in the current position
    void Get_Checks(){
      // To update any check masks we should first ask whether the king is in check,
      // then if any enemy piece attacks the square where our king is located this must be a check 
      if (Is_Square_Attacked(Board, Get_Index(Board.White_King), BLACK)){
        Status.White_Check = Get_Check_Mask(Board, WHITE);
      }
      if (Is_Square_Attacked(Board, Get_Index(Board.Black_King), WHITE)){
        Status.Black_Check = Get_Check_Mask(Board, BLACK);
      }
    }

    // A function to update the game's status, this includes updating the en passant square
    // getting new pins if any, and getting new checks if any were found
    void Update(){
      Get_Checks(); 
      Status.Pins = Get_Pins(WHITE);
      Status.Pins.insert(Get_Pins(BLACK).begin(), Get_Pins(BLACK).end());
    }
};
