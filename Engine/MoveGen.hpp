#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP
#include <vector>
#include "Game.hpp"

vector<Move> Build_Moves(const Game& game, Bitboard MoveBoard, const Index& From, const Piece& Pc, const Piece& Promo_Piece);
vector<Move> Gen_Promo_Moves(const Game& game, Bitboard Pawn_Location);
bool Valid_Move(const Game& game, const Move& move);
vector<Move> Validate(const Game& game, const vector<Move>& Moves);
vector<Move> Verify_Moves_Check(Game& game, const vector<Move>& Moves);
vector<Move> Generate_Slider_Moves(const Game& game, const Piece& Pc);
vector<Move> Generate_Pawn_Moves(Game& game);
vector<Move> Generate_Knight_Moves(const Game &game);
vector<Move> Generate_Bishop_Moves(const Game &game);
vector<Move> Generate_Rook_Moves(const Game &game);
vector<Move> Generate_Queen_Moves(const Game &game);
vector<Move> Generate_King_Moves(Game &game);
vector<Move> Generate_Moves(Game &game);
#endif
