#ifndef MOVES_HPP
#define MOVES_HPP
#include <array>
#include "Utility/Utils.hpp"

extern const array<Bitboard, 64> KING_MOVES;
extern const array<Bitboard, 64> KNIGHT_MOVES;
extern const array<Bitboard, 64> WHITE_PAWN_MOVES;
extern const array<Bitboard, 64> BLACK_PAWN_MOVES;
extern const array<Bitboard, 64> WHITE_PAWN_ATKS;
extern const array<Bitboard, 64> BLACK_PAWN_ATKS;

Bitboard Sliding_Moves(const Bitboard& Location, const Bitboard& Blockers, const Bitboard& Mask);
Bitboard Compute_Rook_attacks(const Bitboard& Location, const Bitboard& Blockers);
Bitboard Compute_Bishop_attacks(const Bitboard& Location, const Bitboard& Blockers);
Bitboard Compute_Queen_attacks(const Bitboard& Location, const Bitboard& Blockers);
#endif
