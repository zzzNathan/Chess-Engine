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
vector<Move> Build_Moves(const Game& game, Bitboard MoveBoard, const Index& From, const Piece& Pc, const Piece& Promo_Piece)
{
  vector<Move> Moves;
  
  const Bitboard& Enemy_Pieces = (game.Current_State.Side == WHITE ? game.B_All : game.W_All);

  Bitboard Current_Move;
  bool     Capture;
  Piece    Capture_Pc = No_Piece;
  bool     Promo      = (Promo_Piece != No_Piece); 
  bool     En_Passant;

  // Loop over all bits in the move board and add their move objects to the vector
  while (MoveBoard)
  {
    Current_Move = Get_LSB(MoveBoard);
    Capture      = (Current_Move & Enemy_Pieces);
    Capture_Pc   = game.Board[Get_Index(Current_Move)];
    En_Passant   = (Get_Index(Current_Move) == game.Current_State.En_Passant);
    
    Moves.push_back(Move(From, Get_Index(Current_Move), Pc, Capture, Capture_Pc, Promo, Promo_Piece, En_Passant));

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
    // Add all promotion possibilities
    for (const Piece& Pc : Promo_Pieces)
      Moves.push_back(Move(Pawn_Index, Get_Index(Above_1), Our_Pawn, false, No_Piece, false, Pc, false));
  }

  // Generate capture promotions
  const Bitboard& Enemy_Pieces           = (game.Current_State.Side == WHITE ? game.B_All : game.W_All);
  const array<Bitboard, 64>& AttackTable = (game.Current_State.Side == WHITE ? WHITE_PAWN_ATKS : BLACK_PAWN_ATKS);
  Bitboard Capture_Promos                = AttackTable[Pawn_Index] & Enemy_Pieces;

  vector<Move> Promo_Moves;

  // Add all capture promotions possibilities
  for (const Piece& Pc : Promo_Pieces)
  {
    vector<Move> Promo_Moves = Build_Moves(game, Capture_Promos, Pawn_Index, Our_Pawn, Pc);
    Moves.insert(Moves.end(), Promo_Moves.begin(), Promo_Moves.end());
  }

  return Moves;
}

// A function that checks whether a move is valid e.g. if the piece is pinned
// or if there is a check
bool Valid_Move(const Game& game, const Move& move)
{
  // Check if the piece moving is pinned
  if (game.Pin_Mask.find(move.From) != game.Pin_Mask.end())
  {
    // If it moves within the pinmask then its a valid move
    if (move.To & game.Pin_Mask.at(move.From)) return true;
    else                                       return false;
  }

  // Check if there is a check in this position
  Bitboard Check_Mask = (game.Current_State.Side == WHITE ? game.White_check_mask : game.Black_check_mask);

  // If the move is within the check mask then it is a valid move
  if (Index_To_Bitboard(move.To) & Check_Mask) return true;

  return false;
}

// A function that takes in a vector of moves and returns a vector of only the 
// moves that are legal
vector<Move> Validate(const Game& game, const vector<Move>& Moves)
{
  vector<Move> Valid;
  
  // Loop through all moves and if it is a valide move then add it to our
  // new vector
  for (const Move& Curr_move : Moves)
    if (Valid_Move(game, Curr_move)) Valid.push_back(Curr_move);

  return Valid;
}

vector<Move> Generate_Pawn_Moves(Game& game)
{
  vector<Move> Moves, Quiet_Moves, Promo_Moves, Capture_Moves, En_Passant_Moves;

  const bool& side       = game.Current_State.Side;
  Bitboard    Pawn_Board = (side == WHITE ? game.W_Pawn : game.B_Pawn);
  const Piece Our_Pawn   = (side == WHITE ? White_Pawn : Black_Pawn);
  const Piece Their_Pawn = (side == WHITE ? Black_Pawn : White_Pawn);
  Bitboard    Curr_Pawn; Index Curr_Index;
  Bitboard    Move_Board;

  const Bitboard&            Promo_Rank   = (side == WHITE ? Rank7 : Rank2);
  const Bitboard&            Enemy_Pieces = (side == WHITE ? game.B_All : game.W_All);
  const array<Bitboard, 64>& MoveTable    = (side == WHITE ? WHITE_PAWN_MOVES : BLACK_PAWN_MOVES);
  const array<Bitboard, 64>& AttackTable  = (side == WHITE ? WHITE_PAWN_ATKS  : BLACK_PAWN_ATKS);
  
  // Loop over all pawns and generate moves for each

  while (Pawn_Board)
  {
    Curr_Pawn  = Get_LSB(Pawn_Board);
    Curr_Index = Get_Index(Curr_Pawn);
    
    // If pawn is on the promotion rank generate all promotion moves
    if (Curr_Pawn & Promo_Rank)
    {
      Promo_Moves = Gen_Promo_Moves(game, Curr_Pawn);
      Moves.insert(Moves.end(), Promo_Moves.begin(), Promo_Moves.end());
      Pawn_Board = Remove_Bit(Pawn_Board, Get_Index(Curr_Pawn)); 
      continue;
    }

    // Generate quiet moves
    Move_Board = MoveTable[Curr_Index];

    // If there is an obstruction 1 square above we can't move
    if (game.Board[Index_Up(Curr_Index, side)] != No_Piece)
      Move_Board = NoBits;

    // If there is an obstruction 2 squares above then we can't move 2 up
    if (game.Board[Index_Up_2(Curr_Index, side)] != No_Piece)
      Move_Board = Remove_Bit(Move_Board, Index_Up_2(Curr_Index, side));
    
    Quiet_Moves = Build_Moves(game, Move_Board, Curr_Index, Our_Pawn, No_Piece);
    Moves.insert(Moves.end(), Quiet_Moves.begin(), Quiet_Moves.end());

    // Generate capture moves
    Move_Board = AttackTable[Curr_Index] & Enemy_Pieces;

    // If there is an en-passant square we are able to capture
    // then we first play the move onto the board to check that our king
    // isn't left in check then if our king isn't in check we may add this move
    if (Move_Board & Index_To_Bitboard(game.Current_State.En_Passant))
    {
      Index King_Index = (side == WHITE ? Get_Index(game.W_King) : Get_Index(game.B_King));
      
      // Make the dummy move
      game.Remove_Piece(Their_Pawn, game.Current_State.En_Passant);
      game.Move_Piece(Our_Pawn, Curr_Index, Index_Up(game.Current_State.En_Passant, side));
      game.Update_Occupancy();
      
      // If there is a check after playing this move then the en-passant would be illegal
      if (!game.Is_Square_Attacked(King_Index, !side))
      {
        En_Passant_Moves = Build_Moves(game, Move_Board & Index_To_Bitboard(game.Current_State.En_Passant), 
                                       Curr_Index, Our_Pawn, No_Piece);
      }

      // Reset boards to how they were before
      game.Place_Piece(Their_Pawn, game.Current_State.En_Passant);
      game.Move_Piece(Our_Pawn, Index_Up(game.Current_State.En_Passant, side), Curr_Index);
      game.Update_Occupancy();

      Move_Board = Remove_Bit(Move_Board, game.Current_State.En_Passant); // We've finished dealing with the move
    }

    // Add all the capture moves
    Capture_Moves = Build_Moves(game, Move_Board, Curr_Index, Our_Pawn, No_Piece);
    Capture_Moves.insert(Capture_Moves.end(), En_Passant_Moves.begin(), En_Passant_Moves.end());

    Pawn_Board = Remove_Bit(Pawn_Board, Get_Index(Curr_Pawn)); 
  }
  
  return Validate(game, Moves);
}

vector<Move> Generate_Moves(Game &game)
{
  // If there is a double check then only the king may move
  if (game.Double_Check) return Generate_King_Moves(game);

  vector<Move> Moves;

  // Generate all moves
  vector<Move> Pawn_Moves   = Generate_Pawn_Moves(game);
  vector<Move> Knight_Moves = Generate_Knight_Moves(game);
  vector<Move> Bishop_Moves = Generate_Bishop_Moves(game);
  vector<Move> Rook_Moves   = Generate_Rook_Moves(game);
  vector<Move> Queen_Moves  = Generate_Queen_Moves(game);
  vector<Move> King_Moves   = Generate_King_Moves(game);

  // Add them all to one vector
  Moves.insert(Moves.end(), Pawn_Moves.begin(),   Pawn_Moves.end());
  Moves.insert(Moves.end(), Knight_Moves.begin(), Knight_Moves.end());
  Moves.insert(Moves.end(), Bishop_Moves.begin(), Bishop_Moves.end());
  Moves.insert(Moves.end(), Rook_Moves.begin(),   Rook_Moves.end());
  Moves.insert(Moves.end(), Queen_Moves.begin(),  Queen_Moves.end());
  Moves.insert(Moves.end(), King_Moves.begin(),   King_Moves.end());

  return Moves;
}
