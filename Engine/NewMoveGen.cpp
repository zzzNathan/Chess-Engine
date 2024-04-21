#include <vector>
#include "NewMoveGen.hpp"
#include "NewGame.hpp"

using namespace std;

// Slider piece movement
// ----------------------
// Implementation of hyperbola quintessence for sliding move generation
// https://www.chessprogramming.org/Hyperbola_Quintessence
Bitboard Sliding_Moves(const Bitboard& Location, const Bitboard& Blockers, const Bitboard& Mask)
{
  Bitboard o, r;
  o  = Blockers & Mask;
  r  = Reverse_bits(o);
  o -= Location;
  r -= Reverse_bits(Location);
  o ^= Reverse_bits(r);
  o &= Mask;

  return o;
}

Bitboard Compute_Rook_attacks(const Bitboard& Location, const Bitboard& Blockers)
{
  Bitboard SquareNum = Get_Index(Location);
  Bitboard Rank = (SquareNum / 8) + 1;
  Bitboard File = SquareNum % 8;
  return (Sliding_Moves(Location, Blockers, RANKS[Rank]) |
          Sliding_Moves(Location, Blockers, FILES[File]) );
}

Bitboard Compute_Bishop_attacks(const Bitboard& Location, const Bitboard& Blockers)
{
  Bitboard SquareNum = Get_Index(Location);
  return (Sliding_Moves(Location, Blockers, DIAGS[SquareNum]) |
          Sliding_Moves(Location, Blockers, ANTI_DIAGS[SquareNum]) );
}

Bitboard Compute_Queen_attacks(const Bitboard& Location, const Bitboard& Blockers)
{
  Bitboard SquareNum = Get_Index(Location);
  Bitboard Rank = (SquareNum / 8) + 1;
  Bitboard File = SquareNum % 8;
  return (Sliding_Moves(Location, Blockers, RANKS[Rank]) |
          Sliding_Moves(Location, Blockers, FILES[File]) |
          Sliding_Moves(Location, Blockers, DIAGS[SquareNum]) |
          Sliding_Moves(Location, Blockers, ANTI_DIAGS[SquareNum]));
}

// General move generation
// ------------------------
// A function to create a vector of moves from a move board, where
// a "move board" is a bitboard of all possible moves for a piece
template<const Piece& Promo_Piece>
vector<Move> Build_Moves(const Game& game, Bitboard MoveBoard, const Index& From, const Piece& Pc)
{
  vector<Move> Moves;
  
  const Bitboard& Enemy_Pieces = (game.Current_State.Side == WHITE ? game.B_All : game.W_All);

  Bitboard Current_Move;
  bool     Capture;
  Piece    Capture_Pc = No_Piece;
  bool     Promo      = (Promo_Piece != No_Piece); 

  // Loop over all bits in the move board and add their move objects to the vector
  while (MoveBoard)
  {
    Current_Move = Get_LSB(MoveBoard);
    Capture      = (Current_Move & Enemy_Pieces);
    Capture_Pc   = game.Board[Get_Index(Current_Move)];

    Moves.push_back(Move(From, Get_Index(Current_Move), Pc, Capture, Capture_Pc, Promo, Promo_Piece, false));

    MoveBoard = Remove_Bit(MoveBoard, Get_Index(Current_Move));
  }

  return Moves;
}

// A function to generate possible promotion pawn moves for the side to play
vector<Move> Gen_Promo_Moves(const Game& game, Bitboard Pawn_Location)
{
  vector<Move>       Moves;
  const static Piece Promo_Pieces[4] = {White_Knight, White_Bishop, White_Rook, White_Queen};
  const Piece&       Our_Pawn        = (game.Current_State.Side == WHITE ? White_Pawn : Black_Pawn);
  const Index        Pawn_Index      = Get_Index(Pawn_Location);
  
  // Generate non-capture promotions
  Bitboard Above_1 = Shift_Up(Pawn_Location, game.Current_State.Side);

  if (!(Above_1 & game.All_Pieces))
  {
    for (const Piece& Pc : Promo_Pieces)
      Moves.push_back(Move(Pawn_Index, Get_Index(Above_1), Our_Pawn, false, No_Piece, false, Pc, false));
  }

  // Generate capture promotions
  const Bitboard& Enemy_Pieces           = (game.Current_State.Side == WHITE ? game.B_All : game.W_All);
  const array<Bitboard, 64>& AttackTable = (game.Current_State.Side == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS);
  Bitboard Capture_Promos                = AttackTable[Pawn_Index] & Enemy_Pieces;

  vector<Move> Promo_Moves;
  for (const Piece& Pc : Promo_Pieces)
  {
    vector<Move> Promo_Moves = Build_Moves<Pc>(game, Capture_Promos, Pawn_Index, Our_Pawn);
    Moves.insert(Moves.end(), Promo_Moves.begin(), Promo_Moves.end());
  }

  return Moves;
}
