#ifndef GAME_HPP
#define GAME_HPP
#include <sstream>
#include <cassert>
#include "Moves.hpp"
//#define NDEBUG

using namespace std;

// This structure will organise all information about a move
// ----------------------------------------------------------
// We will use the following convention for piece promotions:
// - 12 = No promotion
// - 1  = Knight
// - 2  = Bishop
// - 3  = Rook
// - 4  = Queen
//
// We will use the following convention for piece indicators:
// - 0 = White Pawn    |  -  6 = Black Pawn
// - 1 = White Knight  |  -  7 = Black Knight
// - 2 = White Bishop  |  -  8 = Black Bishop
// - 3 = White Rook    |  -  9 = Black Rook
// - 4 = White Queen   |  - 10 = Black Queen
// - 5 = White King    |  - 11 = Black King
//                     |  - 12 = No piece
//
// We will use the number 64 in attributes "To" and "From" to represent
// a null move, a move that is nothing used as the "Last_Move" attribute
// of a game class once it has just been initialised
struct Move
{
  // Attributes
  Index From;          Index To;
  Piece Piece_type;    bool  Capture;
  Piece Capture_PC;    bool  Castle;
  Piece Promoted_PC;   bool  En_Passant;  

  // Constructor to initialise a move
  Move(Index From_, Index To_, Piece Piece_type_, bool Capture_, Piece Capture_PC_, 
       bool Castle_, Piece Promoted_PC_, bool En_Passant_)
  {
    From        = From_;          To         = To_;
    Piece_type  = Piece_type_;    Capture    = Capture_;
    Capture_PC  = Capture_PC_;    Castle     = Castle_;
    Promoted_PC = Promoted_PC_;   En_Passant = En_Passant_;
  }

  // A function to turn move objects into UCI formatted moves
  string UCI() const
  {
    string uci = Index_To_Square[From] + Index_To_Square[To];

    if (Promoted_PC != No_Piece) uci += Piece_To_Uci[Promoted_PC];

    return uci;
  }
};

// This structure will organise all information about a game's status
// -------------------------------------------------------------------
// - Side: White is "true" , Black is "false" 
//
// - En_Passant: Index of the en passant square (will be set to 64 if no en passant square)
//
// - Ply: Halfmove clock, increments after any player's move reset after pawn moves or captures
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
// - Current_State: 1 is white has won, 0 means draw, -1 means black has won and 2 means game is ongoing
struct Status
{
  // Attributes
  bool    Side;
  Index   En_Passant;
  int     Ply;
  int     Fullmove;
  uint8_t Castle_Rights;
  State   Current_State;

  // Constructor
  void Init_Status(const bool& Side_, const Index& En_Passant_, const int& Ply_, 
                   const int& Fullmove_, const uint8_t& Castle_Rights_, const State& Current_State_)
  {
    Side          = Side_;
    En_Passant    = En_Passant_;
    Ply           = Ply_;
    Fullmove      = Fullmove_;
    Castle_Rights = Castle_Rights_;
    Current_State = Current_State_;
  }
};

// This class will organise all information needed to play a game of chess with this engine
// -----------------------------------------------------------------------------------------
class Game
{
  // Attributes
  public:
    // Piece bitboards
    Bitboard W_Pawn;     Bitboard B_Pawn;
    Bitboard W_Knight;   Bitboard B_Knight;
    Bitboard W_Bishop;   Bitboard B_Bishop;
    Bitboard W_Rook;     Bitboard B_Rook;
    Bitboard W_Queen;    Bitboard B_Queen;
    Bitboard W_King;     Bitboard B_King;
    Bitboard W_All;      Bitboard B_All;
    Bitboard All_Pieces; 

    // A list of pointers to all piece bitboards
    Bitboard* PC_Bitboards[12] = {&W_Pawn, &W_Knight, &W_Bishop, &W_Rook, &W_Queen, &W_King,
                                  &B_Pawn, &B_Knight, &B_Bishop, &B_Rook, &B_Queen, &B_King};
  
    // Maps each square index to whatever piece is currently occupying it
    Piece Board[64] = {No_Piece};
    
    // Status of the game
    Status  Current_State;
    Status* Last_State = nullptr;
    
    // Win, draw and loss checking functions 
    // (defined at the end of MoveGen.hpp because these functions rely on move generation)
    State Check_Win();
    State Check_Draw();

    // Last move played (Upon initialisation this will be a null move)
    Move Last_Move = Move(null_sq, null_sq, No_Piece, false, No_Piece, false, No_Piece, false);

    // Masks (see link below for an explanation)
    // Idea from: https://www.codeproject.com/Articles/5313417/Worlds-Fastest-Bitboard-Chess-Movegenerator
    // we will use a bitboard of all bits as 1 to indicate no checks in a position
    unordered_map<Index, Bitboard> Pin_Mask;
    Bitboard White_check_mask;
    Bitboard Black_check_mask;

    // If there is a double check in some position only the king may move
    bool Double_Check = false;

    // Constructor based on a fen string
    // What is a fen?: https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    Game(string& Fen)
    {
      string Piece_Locations, Colour, Castle_Rights, En_Passant, Halfmove, Fullmove;

      // Separates fen into 6 strings each containing the information above 
      stringstream splitter(Fen);
      splitter >> Piece_Locations >> Colour >> Castle_Rights >> En_Passant >> Halfmove >> Fullmove;

      // Initialising piece locations
      // -----------------------------
      Index Square = 63;

      // Iterate over all characters ..
      for (const char& piece : Piece_Locations)
      {
        // If the character is '/' we move to the next row, and if the character
        // is a number we skip that many squares, otherwise we add the piece to the relevant bitboard
        if (piece == '/') continue;
        else if (isdigit(piece)) Square -= Char_To_Int(piece);
        else 
        {
          Place_Piece(Char_To_Piece.at(piece), Square);
          Square--;
        }
      }

      // Initialising game status variables
      // -----------------------------------
      bool    side          = (Colour == "w");
      Index   en_passant    = (En_Passant != "-" ? Square_To_Index.at(En_Passant) : null_sq);
      int     ply           = stoi(Halfmove);
      int     fullmove      = stoi(Fullmove);
      uint8_t castle_rights = 0;

      for (const char& castle : Castle_Rights)
      {
        switch (castle)
        {
          case 'K': { castle_rights |= W_Kingside;  break; }
          case 'Q': { castle_rights |= W_Queenside; break; }
          case 'k': { castle_rights |= B_Kingside;  break; }
          case 'q': { castle_rights |= B_Queenside; break; }
          default:  break;
        }
      }

      Current_State.Init_Status(side, en_passant, ply, fullmove, castle_rights, Ongoing);
      Update(); // Ensure that all pins and check masks have been found
    }

    // Notice how piece indicators perfectly match up to the order of the "PC_Bitboards" array
    // allowing these 2 functions to work with any piece
    
    // Function to place a piece on to the correct board
    void Place_Piece(const Piece& Pc, const Index& Square)
    {
      // De-reference the bitboard at the index given by "Pc" and set the bit to a 1
      *(PC_Bitboards[Pc]) = Set_Bit(*(PC_Bitboards[Pc]), Square);
      Board[Square]       = Pc;
      Update_Occupancy();
    }
    
    // Function to remove a piece from the correct board
    void Remove_Piece(const Piece& Pc, const Index& Square)
    {
      // De-reference the bitboard at the index given by "Pc" and set the bit to a 0
      *(PC_Bitboards[Pc]) = Remove_Bit(*(PC_Bitboards[Pc]), Square);
      Board[Square]       = No_Piece;
      Update_Occupancy();
    }

    // Function to move piece from one square to another
    void Move_Piece(const Piece& Pc, const Index& From, const Index& To)
    {
      Remove_Piece(Pc, From);
      Place_Piece(Pc, To);
    }

    // Function to update the occupancy bitboards
    void Update_Occupancy()
    {
      W_All = W_Pawn | W_Knight | W_Bishop | W_Rook | W_Queen | W_King;
      B_All = B_Pawn | B_Knight | B_Bishop | B_Rook | B_Queen | B_King;

      All_Pieces = W_All | B_All;
    }

    void Make_Move(const Move& move)
    {
      Last_State = &Current_State; // Set the last game status to the current one
      if (Current_State.Side == BLACK) Current_State.Fullmove++; // Fullmove always increments after black's move
      Current_State.Ply++;                                       // Increment ply counter after every move

      // Move piece to new square (promotions are handled below)
      if (move.Promoted_PC == No_Piece) 
        Move_Piece(move.Piece_type, move.From, move.To);
     
      // If we capture en-passant the captured piece will not be on the same square
      // that we are moving to
      if (move.En_Passant)
      {
        // We instead capture the pawn adjacent to our pawn or equally the pawn that is
        // underneath our pawn after making the move
        Index Under_1_Index = Index_Down(move.To, Current_State.Side);
        if (Current_State.Side == WHITE) Remove_Piece(Black_Pawn, Under_1_Index);
        else                             Remove_Piece(White_Pawn, Under_1_Index);
      }

      // If it's a capture we remove the captured piece
      else if (move.Capture)
        Remove_Piece(move.Capture_PC, move.To);
       
      // If it's a promotion we add the new piece to the board
      if (move.Promoted_PC != No_Piece)
      {
        Remove_Piece(move.Piece_type, move.From);
        Place_Piece(move.Promoted_PC, move.To);
      }

      // Update game state variables
      Last_Move = move;
      Update();
      Current_State.Side = !Current_State.Side;
    }

    void Unmake_Move(const Move& move)
    {
      // Set all game state variables to the previous state
      assert(Last_State != nullptr); // Ensure that a last state exists
      
      Current_State = *Last_State;
      Last_State    = nullptr; // We no longer have a last state

      // Unmake the move (promotions are handled below)
      if (move.Promoted_PC == No_Piece)
        Move_Piece(move.Piece_type, move.To, move.From);
      
      // Restore the piece that we captured en-passant
      if (move.En_Passant)
      {
        if (Current_State.Side == WHITE) Place_Piece(Black_Pawn, Current_State.En_Passant);
        else                             Place_Piece(White_Pawn, Current_State.En_Passant);
      }
      
      // Restore the piece that we captured normally
      else if (move.Capture)
        Place_Piece(move.Capture_PC, move.To);
      
      // Restore the piece that was promoted and replace our piece onto it's original square
      if (move.Promoted_PC != No_Piece)
      {
        Remove_Piece(move.Promoted_PC, move.To);
        Place_Piece(move.Piece_type, move.From);
      }

      Update();
    }

    // Is the given square currently under attack by the given colour
    // CREDITS: https://www.chessprogramming.org/Square_Attacked_By
    // "A common approach is to put a super-piece on the to-square, to look up all kind of piece-type
    // attacks from there and to intersect them with all appropriate pieces able to attack that square.
    // Note that white pawn attacks intersect black pawns and vice versa." 
    bool Is_Square_Attacked(const Index& Square, const bool& Colour) const
    {
      // Does a pawn attack this square?
      const array<Bitboard, 64>& Pawn_Attacks = (Colour == WHITE ? BLACK_PAWN_ATKS : WHITE_PAWN_ATKS);
      const Bitboard& Pawns                   = (Colour == WHITE ? W_Pawn : B_Pawn);
      if (Pawn_Attacks[Square] & Pawns) return true;

      // Does a knight attack this square?
      const Bitboard& Knights = (Colour == WHITE ? W_Knight : B_Knight);
      if (KNIGHT_MOVES[Square] & Knights) return true;
      
      // Does a king attack this square?
      const Bitboard& King = (Colour == WHITE ? W_King : B_King);
      if (KING_MOVES[Square] & King) return true;
      
      // We need to remove the 1 bit of this square from the occupancy (if it exists)
      // otherwise hyperbola quintessence will generate moves incorrectly
      const Bitboard Occupancy = Remove_Bit(All_Pieces, Square);

      // Does a rook or a queen attack this square?
      const Bitboard Rook_Queen = (Colour == WHITE ? W_Rook | W_Queen : B_Rook | B_Queen); 
      if (Compute_Rook_attacks(Index_To_Bitboard(Square), Occupancy) & Rook_Queen) return true;

      // Does a bishop or a queen attack this square?
      const Bitboard Bishop_Queen = (Colour == WHITE ? W_Bishop | W_Queen : B_Bishop | B_Queen);
      if (Compute_Bishop_attacks(Index_To_Bitboard(Square), Occupancy) & Bishop_Queen) return true;

      return false;
    }

    // Function to get the FEN of the game used for debugging
    string Get_Fen() const
    {
      string fen = "";
      char   piece;
      short  empty_count = 0; // How many empty squares have we encountered in a row
      
      // Piece placement data
      // ---------------------
      // Loop over all squares .. 
      for (int sq = a8; sq >= h1; sq--)
      {
        piece = Board[sq];

        // If the square is empty increment the empty count
        if (piece == No_Piece) empty_count++;

        // Otherwise if the square is not empty then add the piece and reset the empty count
        else
        { 
          if (empty_count > 0) fen += to_string(empty_count);
          fen        += Piece_To_Char[piece];
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
      fen += (Current_State.Side == WHITE ? " w " : " b ");

      // Castling availability
      // ----------------------
      if (Current_State.Castle_Rights == No_Castle) fen += "-";
      else
      {
        if (Current_State.Castle_Rights & W_Kingside)  fen += "K";
        if (Current_State.Castle_Rights & W_Queenside) fen += "Q";
        if (Current_State.Castle_Rights & B_Kingside)  fen += "k";
        if (Current_State.Castle_Rights & B_Queenside) fen += "q";
      }

      // En passant square
      // ------------------
      if (Current_State.En_Passant == null_sq) fen += " - ";
      else                                     fen += " " + Index_To_Square[Current_State.En_Passant] + " ";

      // Halfmove and fullmove
      // ----------------------
      fen += to_string(Current_State.Ply) + " " + to_string(Current_State.Fullmove);
      
      return fen;
    }

    void Show_Board() const
    {  
      int rank = 8;

      cout << "\n" << rank << " | ";
      
      // Loop over all squares and print the piece on this square
      for (Index sq = a8; sq >= h1; sq--)
      {
        cout << (Board[sq] != No_Piece ? Piece_To_Char[Board[sq]] : '.') << " "; 

        // If this the end of a rank print a new line
        if (sq % 8 == 0) cout << "\n" << --rank << " | ";
      }

      // Print letters
      cout << "    _______________\n";
      cout << "    a b c d e f g h\n\n";
    }

  private:
    // Function to get check masks
    void Get_Check_mask(const bool& Colour)
    {
      Bitboard mask; 
      short    checks = 0; // Used to count the number of checks found
      const Bitboard& King_Location = (Colour == WHITE ? W_King : B_King);
      const Index&    King_Index    = Get_Index(King_Location);

      // Enemy pieces variables
      const Bitboard& Enemy_Pawns         = (Colour == WHITE ? B_Pawn   : W_Pawn);
      const Bitboard& Enemy_Knights       = (Colour == WHITE ? B_Knight : W_Knight);
      const Bitboard  Enemy_BishopsQueens = (Colour == WHITE ? B_Bishop | B_Queen : W_Bishop | W_Queen);
      const Bitboard  Enemy_RooksQueens   = (Colour == WHITE ? B_Rook | B_Queen   : W_Rook | W_Queen);

      const array<Bitboard, 64>& Pawn_Attacks = (Colour == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS); 

      // Non-slider pieces
      if (Pawn_Attacks[King_Index] & Enemy_Pawns)   { mask = Pawn_Attacks[King_Index] & Enemy_Pawns;   checks++; }
      if (KNIGHT_MOVES[King_Index] & Enemy_Knights) { mask = KNIGHT_MOVES[King_Index] & Enemy_Knights; checks++; }

      // We need to remove the 1 bit from the occupancy (if it exists)
      // otherwise hyperbola quintessence will generate moves incorrectly
      const Bitboard Occupancy = Remove_Bit(All_Pieces, King_Index);

      // Slider pieces
      Bitboard Attacker;
      if (Compute_Bishop_attacks(King_Location, Occupancy) & Enemy_BishopsQueens)
      {
        Attacker = Compute_Bishop_attacks(King_Location, Occupancy) & Enemy_BishopsQueens;
        mask     = Remove_Bit(Create_Ray(King_Location, Attacker), King_Index); // Can't move onto the king
        checks++;
      }

      if (Compute_Rook_attacks(King_Location, Occupancy) & Enemy_RooksQueens)
      {
        Attacker = Compute_Rook_attacks(King_Location, Occupancy) & Enemy_RooksQueens;
        mask     = Remove_Bit(Create_Ray(King_Location, Attacker), King_Index); // Can't move onto the king
        checks++; 
      }
      
      Bitboard& Check_Mask = (Colour == WHITE ? White_check_mask : Black_check_mask);

      if (checks >= 2) Double_Check = true; 
      else
      {
        Double_Check = false;
        Check_Mask   = (checks == 1 ? mask : NONE);
      }
    }

    // A function to find pins in the current position
    // To generate any pin masks we should notice that only sliding pieces
    // may pin other pieces, with this in mind consider getting the move 
    // bitboard of a queen from the square where the king is located.
    // If the only 2 pieces are the pinner then the 'pinnee' AND the pinner is 
    // in front of the 'pinnee' then we have found a pin. 'Pinnee' refers to a piece
    // that is being pinned.
    unordered_map<Index, Bitboard> Get_Pins(const bool& Colour) const
    {
      unordered_map<Index, Bitboard> Pins;

      // Get working variables
      const Bitboard& FriendlyKing = (Colour == WHITE ? W_King   : B_King);
      const Bitboard& EnemyQueen   = (Colour == WHITE ? B_Queen  : W_Queen);
      const Bitboard& EnemyRook    = (Colour == WHITE ? B_Rook   : W_Rook);
      const Bitboard& EnemyBishop  = (Colour == WHITE ? B_Bishop : W_Bishop);
      
      // Get locations of enemy slider pieces aligned with our king these will be
      // our potential pinners 
      Bitboard Pinners = (
        // Enemy pieces on diagonal rays
        (Compute_Bishop_attacks(FriendlyKing, NoBits) & (EnemyQueen | EnemyBishop)) |

        // Enemy pieces on horizontal and vertical rays
        (Compute_Rook_attacks(FriendlyKing,   NoBits) & (EnemyQueen | EnemyRook))
      );

      // Iterate through all potential pinners and check if a pinned piece is on this ray
      Bitboard Attacker, Pinnee;
      while (Pinners)
      {
        Attacker = Get_LSB(Pinners);

        // Get potential 'pinnee's, by building a ray from the attacker to the king
        Pinnee = Create_Ray(FriendlyKing, Attacker) & All_Pieces;  
        Pinnee = Remove_Bit(Pinnee, Get_Index(FriendlyKing)); // The king can never be pinned
        Pinnee = Remove_Bit(Pinnee, Get_Index(Attacker));   // The attacker can never be pinned

        // There can only be on pinned piece along a ray otherwise this wouldn't be a pin
        if (BitCount(Pinnee) == 1)
        {
          // Then the only moves that this piece may make is along this ray or to capture the attacker
          Pins[Get_Index(Pinnee)] = Create_Ray(FriendlyKing, Attacker);
          Pins[Get_Index(Pinnee)] = Remove_Bit(Pins[Pinnee], Get_Index(FriendlyKing)); // Can't capture the king
        }

        Pinners = Remove_Bit(Pinners, Get_Index(Attacker));
      }

      return Pins; 
    }

    // A function to update the current en-passant square
    void Get_En_Passant()
    {
      // If there is no last move then we do not need to update the en-passant square
      if (Last_Move.From == null_sq) return;

      // If the last move was not a pawn then we can set the en-passant square to all bits
      if (Last_Move.Piece_type != White_Pawn || Last_Move.Piece_type != Black_Pawn)
      {
        Current_State.En_Passant = null_sq;
        return;
      }

      // If the last move was a double pawn push then we can update the en passant square
      // New en-passant square will be the square underneath the pawn that was just pushed
      const int Difference = Last_Move.To - Last_Move.From;    
      if (abs(Difference) == 16)
      {
        if (Current_State.Side == WHITE) Current_State.En_Passant = Shift_Down(Last_Move.To, WHITE); 
        else                             Current_State.En_Passant = Shift_Down(Last_Move.To, BLACK);
      }
    }

    // A function to update castling rights
    void Get_Castle_Rights()
    {
      const Index& King_rights  = (Current_State.Side == WHITE ? W_Kingside  : B_Kingside);
      const Index& Queen_rights = (Current_State.Side == WHITE ? W_Queenside : B_Queenside);

      const Index& Enemy_King_rights  = (Current_State.Side == WHITE ? B_Kingside  : W_Kingside);
      const Index& Enemy_Queen_rights = (Current_State.Side == WHITE ? B_Queenside : W_Queenside);

      // If a rook was just captured then we lose this sides castling rights
      if (Last_Move.Capture)
      {
        if ((Current_State.Side == WHITE && Last_Move.To == h8) || (Current_State.Side == BLACK && Last_Move.To == h1))
          Current_State.Castle_Rights &= ~Enemy_King_rights;

        if ((Current_State.Side == WHITE && Last_Move.To == a8) || (Current_State.Side == BLACK && Last_Move.To == a1))
          Current_State.Castle_Rights &= ~Enemy_Queen_rights;
      } 

      // If there is no last move or the last move wasn't from the king
      // or rook we dont need to update the castle rights
      Piece Our_King = (Current_State.Side == WHITE ? White_King : Black_King);
      Piece Our_Rook = (Current_State.Side == WHITE ? White_Rook : Black_Rook);
      if (Last_Move.From  == null_sq || 
         (Last_Move.Piece_type != Our_King && Last_Move.Piece_type != Our_Rook)) return;

      // Get working variables
      const bool King_Side = (Last_Move.To < Last_Move.From); // See the diagram in "Constant.hpp"
      const int  Rook_To   = (King_Side ? Index_Right(Last_Move.From) : Index_Left(Last_Move.From));
      const int  Rook_From = (King_Side ? Index_Right(Last_Move.To)   : Index_Left_2(Last_Move.To));
      
      // If the last move was a rook moving for the first time we remove
      // castle rights for that side
      if (Last_Move.Piece_type == Our_Rook)
      {
        if ((Current_State.Side == WHITE && Last_Move.From == h1) || (Current_State.Side == BLACK && Last_Move.From == h8))
          Current_State.Castle_Rights &= ~King_rights;

        if ((Current_State.Side == WHITE && Last_Move.From == a1) || (Current_State.Side == BLACK && Last_Move.From == a8))
          Current_State.Castle_Rights &= ~Queen_rights;
        return;
      }
      
      // We lose all castling rights if the last move was from the king
      Current_State.Castle_Rights &= ~(King_rights | Queen_rights);

      // If the last move was by a king that didn't castle we don't have to move the rook
      if (abs(Last_Move.From - Last_Move.To) != 2) return;

      Move_Piece(Our_Rook, Rook_From, Rook_To);
    }

    // A function to update the game's status, this includes updating the en passant square
    // getting new pins if any, and getting new checks if any were found and more
    void Update()
    {
      Get_Check_mask(WHITE);
      Get_Check_mask(BLACK);

      Get_En_Passant();

      Pin_Mask = Get_Pins(WHITE);
      Pin_Mask.insert(Get_Pins(BLACK).begin(), Get_Pins(BLACK).end());

      if (Last_Move.Capture || Last_Move.Piece_type == White_Pawn || Last_Move.Piece_type == Black_Pawn) 
        Current_State.Ply = 0; // Reset ply if there was a capture or pawn move
    }
};
#endif
