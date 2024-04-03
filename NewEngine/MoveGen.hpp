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
#include "Game.h"
#include <vector>

using namespace std;
typedef unsigned long long i64;

// A function to generate possible promotion pawn moves for the side to play
vector<Move> Get_Promo_Moves(Game CurrGame, i64 CurrPawn){
  vector<Move> Moves;
  
  // Non-capture promotions 
  i64 Above_1 = Shift_Up(CurrPawn, CurrGame.Status.Side);
  if (!(Above_1 & CurrGame.Board.All_Pieces)){ // If there is an empty square above
    Moves.push_back(Move(CurrPawn, Above_1, PAWN, false, KNIGHT, false)); 
    Moves.push_back(Move(CurrPawn, Above_1, PAWN, false, BISHOP, false)); 
    Moves.push_back(Move(CurrPawn, Above_1, PAWN, false, ROOK,   false)); 
    Moves.push_back(Move(CurrPawn, Above_1, PAWN, false, QUEEN,  false)); 
  }

  // Capture promotions
  const array<i64, 64> AttackTable = (CurrGame.Status.Side == WHITE ? WHITE_PAWN_ATKS  : BLACK_PAWN_ATKS);
  i64 EnemyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.Black_All : CurrGame.Board.White_All);
  i64 Capture_Promos = AttackTable[Get_Index(CurrPawn)] & EnemyPieces;

  // Loop over all capture promotions and add their moves to the vector
  i64 CurrCapture;
  while (Capture_Promos){

    CurrCapture = Get_LSB(Capture_Promos);
    
    Moves.push_back(Move(CurrPawn, CurrCapture, PAWN, true, KNIGHT, false));
    Moves.push_back(Move(CurrPawn, CurrCapture, PAWN, true, BISHOP, false));
    Moves.push_back(Move(CurrPawn, CurrCapture, PAWN, true, ROOK,   false));
    Moves.push_back(Move(CurrPawn, CurrCapture, PAWN, true, QUEEN,  false));

    Capture_Promos ^= CurrCapture; // Remove the bit from capture promotions
  }

  return Moves;
}

// A function to create a vector of moves from a move board
vector<Move> Create_Moves(i64 MoveBoard, i64 From, i64 Piece, bool Capture){
  vector<Move> Moves;

  i64 To; // The square the piece is moving to
  while (MoveBoard)
  {
    To = Get_LSB(MoveBoard);
    Moves.push_back(Move(From, To, Piece, Capture, NO_PROMO, false));
    MoveBoard ^= To; // Remove the bit from the move board
  }
  return Moves;
}

// A function to create a vector of moves from a move board when we aren't sure 
// whether this is move is a capture or not
vector<Move> Build_Moves(Game CurrGame, i64 MoveBoard, i64 From, i64 Piece){
  vector<Move> Moves;
  
  i64 EnemyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.Black_All : CurrGame.Board.White_All);
  i64 To; // The square the piece is moving to
  bool Capture;
  while (MoveBoard)
  {
    To = Get_LSB(MoveBoard);
    Capture = (To & EnemyPieces ? true : false);
    Moves.push_back(Move(From, To, Piece, Capture, NO_PROMO, false));
    MoveBoard ^= To; // Remove the bit from the move board 
  }

  return Moves;
}

// Verify that none of the moves passed in came from pinned pieces that move
// off the attacking ray 
vector<Move> Verify_Moves_Pins(Game CurrGame, vector<Move> Moves){
  vector<Move> Valid_Moves;
  
  // Loop over all moves ..
  for (Move m : Moves)
  {
    // .. and if the piece is in the pins map then check that
    // it moves through the pin mask, if not then the move isn't legal 
    if (CurrGame.Status.Pins.find(m.From) != CurrGame.Status.Pins.end()){

      if (m.To & CurrGame.Status.Pins[m.From]) Valid_Moves.push_back(m);
    }
    // If the piece isn't pinned then the move is legal
    else Valid_Moves.push_back(m);
  }

  return Valid_Moves;
}

// Verify that none of the moves passed in fail to block check (if there is a check)
vector<Move> Verify_Moves_Check(Game CurrGame, vector<Move> Moves){
  vector<Move> Valid_Moves;
  
  i64 Check_Mask = (CurrGame.Status.Side == WHITE ? CurrGame.Status.White_Check : CurrGame.Status.Black_Check);
  if (Check_Mask == AllBits) return Moves; // Nothing to do if there is no check

  // Loop over all moves and if they move within the check mask this is a legal move
  for (Move m : Moves)
  {
    if (m.To & Check_Mask) Valid_Moves.push_back(m);
  }

  return Valid_Moves; 
}

// A function to generate possible pawn moves for the side to play
vector<Move> Generate_Pawn_Moves(Game CurrGame){
  vector<Move> Moves;
  
  // Get correct pawn bitboard and relevant tables
  i64 PawnBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_Pawn : CurrGame.Board.Black_Pawn);
  const array<i64, 64> MoveTable   = (CurrGame.Status.Side == WHITE ? WHITE_PAWN_MOVES : BLACK_PAWN_MOVES);
  const array<i64, 64> AttackTable = (CurrGame.Status.Side == WHITE ? WHITE_PAWN_ATKS  : BLACK_PAWN_ATKS);

  i64 EnemyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.Black_All : CurrGame.Board.White_All);
  
  // Loop over all pawns and generate their legal moves
  vector<Move> Moves_To_Add; i64 MoveBoard;
  while (PawnBB)
  {
    MoveBoard = 0;
    i64 CurrPawn = Get_LSB(PawnBB);

    // Capture pawn moves
    // -------------------
    Moves_To_Add = Create_Moves(AttackTable[Get_Index(CurrPawn)] & EnemyPieces, CurrPawn, PAWN, true); 
    Moves.insert(Moves.end(), Moves_To_Add.begin(), Moves_To_Add.end());
    
    // If an en-passant square exists and we are able to attack it then add this move
    if ((CurrGame.Status.En_Passant != AllBits) && (AttackTable[Get_Index(CurrPawn)] & CurrGame.Status.En_Passant)){
      Moves.push_back(Move(CurrPawn, CurrGame.Status.En_Passant, PAWN, true, NO_PROMO, true));
    }

    // Quiet pawn moves
    // -----------------
    // Handle promotion moves
    i64 PromoRank = (CurrGame.Status.Side == WHITE ? Rank7 : Rank2);

    if (CurrPawn & PromoRank){
      vector<Move> PromoMoves = Get_Promo_Moves(CurrGame, CurrPawn);
      Moves.insert(Moves.end(), PromoMoves.begin(), PromoMoves.end());
      PawnBB ^= CurrPawn; // Remove the pawn from the bitboard
      continue; // We can then skip the other quiet moves
    }

    // If the square one above isnt obstructed then add this bit
    i64 Above_1 = Shift_Up(CurrPawn, CurrGame.Status.Side);
    if (!(Above_1 & CurrGame.Board.All_Pieces)) MoveBoard |= MoveTable[Get_Index(CurrPawn)];

    // If the square two above is obstructed then remove this bit
    i64 Above_2 = Shift_Up(Above_1, CurrGame.Status.Side);
    if (Above_2 & CurrGame.Board.All_Pieces) MoveBoard = Remove_Bit(MoveBoard, Get_Index(Above_2));
    
    // Add the move
    Moves_To_Add = Create_Moves(MoveBoard, CurrPawn, PAWN, false);
    Moves.insert(Moves.end(), Moves_To_Add.begin(), Moves_To_Add.end());

    PawnBB ^= CurrPawn; // Remove the pawn from the bitboard
  }
  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Pins(CurrGame, Moves);
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;
}

// A function to generate possible knight moves for the side to play
vector<Move> Generate_Knight_Moves(Game CurrGame){
  vector<Move> Moves;
  // Get the correct knight bitboard and relevant attack tables
  i64 KnightBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_Knight : CurrGame.Board.Black_Knight);

  i64 FriendlyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_All : CurrGame.Board.Black_All);
  
  // Loop over all knights and generate their legal moves
  i64 CurrKnight, MoveBB; vector<Move> CurrMoves;
  while (KnightBB)
  {
    CurrKnight = Get_LSB(KnightBB);
    
    MoveBB = KNIGHT_MOVES[Get_Index(CurrKnight)] & (~FriendlyPieces); // Can't capture your own pieces
    CurrMoves = Build_Moves(CurrGame, MoveBB, CurrKnight, KNIGHT);
    Moves.insert(Moves.end(), CurrMoves.begin(), CurrMoves.end());

    KnightBB ^= CurrKnight; // Remove the knight from the bitboard
  }

  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Pins(CurrGame, Moves);
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;
}

vector<Move> Generate_Bishop_Moves(Game CurrGame){
  vector<Move> Moves;
  // Get the correct bishop bitboard and relevant attack tables
  i64 BishopBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_Bishop : CurrGame.Board.Black_Bishop);

  i64 FriendlyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_All : CurrGame.Board.Black_All);

  // Loop over all bishops and generate their legal moves
  i64 CurrBishop, MoveBB, Occupancy; vector<Move> CurrMoves;
  while (BishopBB)
  {
    CurrBishop = Get_LSB(BishopBB);
  
    // The piece moving can't be part of occupancy this leads to incorrect moves from hyperbola quintessence
    Occupancy = Remove_Bit(CurrGame.Board.All_Pieces, Get_Index(CurrBishop));
    MoveBB    = Compute_Bishop_attacks(CurrBishop, Occupancy) & (~FriendlyPieces); // Can't capture your own pieces
    CurrMoves = Build_Moves(CurrGame, MoveBB, CurrBishop, BISHOP);
    Moves.insert(Moves.end(), CurrMoves.begin(), CurrMoves.end());

    BishopBB ^= CurrBishop; // Remove the bishop from the bitboard
  }
  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Pins(CurrGame, Moves);
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;
}

vector<Move> Generate_Rook_Moves(Game CurrGame){
  vector<Move> Moves;
  // Get the correct rook bitboard and relevant attack tables
  i64 RookBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_Rook : CurrGame.Board.Black_Rook);

  i64 FriendlyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_All : CurrGame.Board.Black_All);

  // Loop over all rooks and generate their legal moves
  i64 CurrRook, MoveBB, Occupancy; vector<Move> CurrMoves;
  while (RookBB)
  {
    CurrRook = Get_LSB(RookBB);

    // The piece moving can't be part of occupancy this leads to incorrect moves from hyperbola quintessence
    Occupancy = Remove_Bit(CurrGame.Board.All_Pieces, Get_Index(CurrRook));
    MoveBB    = Compute_Rook_attacks(CurrRook, Occupancy) & (~FriendlyPieces); // Can't capture your own pieces
    CurrMoves = Build_Moves(CurrGame, MoveBB, CurrRook, ROOK);
    Moves.insert(Moves.end(), CurrMoves.begin(), CurrMoves.end());

    RookBB ^= CurrRook; // Remove the rook from the bitboard
  }

  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Pins(CurrGame, Moves);
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;
}

vector<Move> Generate_Queen_Moves(Game CurrGame){
  vector<Move> Moves;
  // Get the correct queen bitboard and relevant attack tables
  i64 QueenBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_Queen : CurrGame.Board.Black_Queen);

  i64 FriendlyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_All : CurrGame.Board.Black_All);

  // Loop over all queens and generate their legal moves
  i64 CurrQueen, MoveBB, Occupancy; vector<Move> CurrMoves;
  while (QueenBB)
  {
    CurrQueen = Get_LSB(QueenBB);

    // The piece moving can't be part of occupancy this leads to incorrect move from hyperbola quintessence
    Occupancy = Remove_Bit(CurrGame.Board.All_Pieces, Get_Index(CurrQueen)); 
    MoveBB    = Compute_Queen_attacks(CurrQueen, Occupancy) & (~FriendlyPieces); // Can't capture your own pieces
    CurrMoves = Build_Moves(CurrGame, MoveBB, CurrQueen, QUEEN);
    Moves.insert(Moves.end(), CurrMoves.begin(), CurrMoves.end());

    QueenBB ^= CurrQueen; // Remove the queen from the bitboard
  }

  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Pins(CurrGame, Moves);
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;  
}

vector<Move> Generate_King_Moves(Game CurrGame){
  vector<Move> Moves;

  // Get the correct king bitboard
  i64 KingBB = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_King : CurrGame.Board.Black_King);

  i64 FriendlyPieces = (CurrGame.Status.Side == WHITE ? CurrGame.Board.White_All : CurrGame.Board.Black_All);
  
  // Normal moves
  // -------------
  i64 MoveBB = KING_MOVES[Get_Index(KingBB)] & (~FriendlyPieces); // Can't capture your own pieces
  vector<Move> CurrMoves = Build_Moves(CurrGame, MoveBB, KingBB, KING);
  Moves.insert(Moves.end(), CurrMoves.begin(), CurrMoves.end());

  // Castling moves
  // ---------------
  i64 Castle_Square = (CurrGame.Status.Side == WHITE ? SquareE1 : SquareE8);
  i64 Check_Mask    = (CurrGame.Status.Side == WHITE ? CurrGame.Status.White_Check : CurrGame.Status.Black_Check);

  // If king isn't on the castling square or there is a check we may return early because castling will be illegal
  if (!(KingBB & Castle_Square) || Check_Mask == AllBits)
  { 
    Moves = Verify_Moves_Check(CurrGame, Moves);
    return Moves;
  }
  
  // Get relevant castling rights
  bool Kingside_Rights, Queenside_Rights;
  if (CurrGame.Status.Side == WHITE)
  {
    Kingside_Rights  = CurrGame.Status.Castle_Rights & W_Kingside;
    Queenside_Rights = CurrGame.Status.Castle_Rights & W_Queenside;
  } 
  else 
  {
    Kingside_Rights  = CurrGame.Status.Castle_Rights & B_Kingside;
    Queenside_Rights = CurrGame.Status.Castle_Rights & B_Queenside;
  }

  bool EnemyCol = !CurrGame.Status.Side; // The colour of the enemy
  
  // We need to make sure that the king doesn't move through check
  // we do this by making sure that the squares between the king and the rook are not under attack
  if (Kingside_Rights){
    i64 Right_1 = (CurrGame.Status.Side == WHITE ? SquareF1 : SquareF8);
    i64 Right_2 = (CurrGame.Status.Side == WHITE ? SquareG1 : SquareG8);
    if ((!(Is_Square_Attacked(CurrGame.Board, Right_1, EnemyCol))) && 
        (!(Is_Square_Attacked(CurrGame.Board, Right_2, EnemyCol)))){

      Moves.push_back(Move(KingBB, Right_2, KING, false, NO_PROMO, false));
    }
  }

  if (Queenside_Rights){
    i64 Left_1 = (CurrGame.Status.Side == WHITE ? SquareD1 : SquareD8);
    i64 Left_2 = (CurrGame.Status.Side == WHITE ? SquareC1 : SquareC8);
    if ((!(Is_Square_Attacked(CurrGame.Board, Left_1, EnemyCol))) && 
        (!(Is_Square_Attacked(CurrGame.Board, Left_2, EnemyCol)))){

      Moves.push_back(Move(KingBB, Left_2, KING, false, NO_PROMO, false));
    }
  }
  // Ensure that we have no illegal moves
  Moves = Verify_Moves_Check(CurrGame, Moves);
  return Moves;
}

// A function to generate all possible moves
vector<Move> Generate_Moves(Game CurrGame){
  vector<Move> Moves;
  
  // Generate all moves
  vector<Move> Pawn_Moves   = Generate_Pawn_Moves(CurrGame);
  vector<Move> Knight_Moves = Generate_Knight_Moves(CurrGame);
  vector<Move> Bishop_Moves = Generate_Bishop_Moves(CurrGame);
  vector<Move> Rook_Moves   = Generate_Rook_Moves(CurrGame);
  vector<Move> Queen_Moves  = Generate_Queen_Moves(CurrGame);
  vector<Move> King_Moves   = Generate_King_Moves(CurrGame);

  // Add them all to one vector
  Moves.insert(Moves.end(), Pawn_Moves.begin(),   Pawn_Moves.end());
  Moves.insert(Moves.end(), Knight_Moves.begin(), Knight_Moves.end());
  Moves.insert(Moves.end(), Bishop_Moves.begin(), Bishop_Moves.end());
  Moves.insert(Moves.end(), Rook_Moves.begin(),   Rook_Moves.end());
  Moves.insert(Moves.end(), Queen_Moves.begin(),  Queen_Moves.end());
  Moves.insert(Moves.end(), King_Moves.begin(),   King_Moves.end());

  return Moves;
}