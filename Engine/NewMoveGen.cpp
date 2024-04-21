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

vector<Move>
