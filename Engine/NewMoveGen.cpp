#include <functional>
#include "NewMoveGen.hpp"

using namespace std;

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

// A function to play dummy moves to test whether or not there is a check after this move
// If there is then the move is illegal and should be removed
vector<Move> Verify_Moves_Check(Game& game, const vector<Move>& Moves)
{
  vector<Move> Valid;
  
  // Loop through all moves and play them onto the board, if the king isn't
  // in check after this move then it's legal
  Index King_sq = (game.Current_State.Side == WHITE ? Get_Index(game.W_King): Get_Index(game.B_King));
  for (const Move& Curr_move : Moves)
  {
    game.Make_Move(Curr_move);

    if (!game.Is_Square_Attacked(King_sq, !game.Current_State.Side))
      Valid.push_back(Curr_move);
    
    game.Unmake_Move(Curr_move);
  }

  return Valid;
}

// A function to generate moves for slider pieces
vector<Move> Generate_Slider_Moves(const Game& game, const Piece& Pc)
{
  vector<Move> Moves;
  
  const Bitboard& Friendly_Pieces = (game.Current_State.Side == WHITE ? game.W_All : game.B_All);

  // We need to get the relevant movement function for the piece
  function<Bitboard(const Bitboard&, const Bitboard&)> Movement;
  if      (Pc == White_Bishop || Pc == Black_Bishop) Movement = Compute_Bishop_attacks;
  else if (Pc == White_Rook   || Pc == Black_Rook)   Movement = Compute_Rook_attacks;
  else if (Pc == White_Queen  || Pc == Black_Queen)  Movement = Compute_Queen_attacks;

  // We also need the bitboard of the piece we are generating moves for
  Bitboard Piece_Board = *(game.PC_Bitboards[Pc]);

  // Then loop over all pieces and generate their legal moves
  Bitboard     Curr_Piece, Move_Board, Occupancy;
  vector<Move> Curr_Moves;

  while (Piece_Board)
  {
    Curr_Piece = Get_LSB(Piece_Board);

    // The piece moving can't be part of occupancy this leads to incorrect moves from the hyperbola quintessence
    Occupancy = Remove_Bit(game.All_Pieces, Get_Index(Curr_Piece));

    Move_Board = Movement(Curr_Piece, Friendly_Pieces) & (~Occupancy); // Can't capture your own pieces
    Curr_Moves = Build_Moves(game, Move_Board, Get_Index(Curr_Piece), Pc, No_Piece);

    Moves.insert(Moves.end(), Curr_Moves.begin(), Curr_Moves.end());

    Piece_Board = Remove_Bit(Piece_Board, Get_Index(Curr_Piece));
  }

  return Validate(game, Moves);
}

vector<Move> Generate_Pawn_Moves(Game& game)
{
  vector<Move> Moves, Quiet_Moves, Promo_Moves, Capture_Moves, En_Passant_Moves;

  const bool&  side       = game.Current_State.Side;
  Bitboard     Pawn_Board = (side == WHITE ? game.W_Pawn : game.B_Pawn);
  const Piece& Our_Pawn   = (side == WHITE ? White_Pawn : Black_Pawn);
  const Piece& Their_Pawn = (side == WHITE ? Black_Pawn : White_Pawn);
  Bitboard     Curr_Pawn; Index Curr_Index;
  Bitboard     Move_Board;

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
      Pawn_Board  = Remove_Bit(Pawn_Board, Get_Index(Curr_Pawn)); 
      Promo_Moves = Gen_Promo_Moves(game, Curr_Pawn);

      Moves.insert(Moves.end(), Promo_Moves.begin(), Promo_Moves.end());
      continue;
    }

    // Generate quiet moves
    // ---------------------
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
    // -----------------------
    Move_Board = AttackTable[Curr_Index] & Enemy_Pieces;

    // If there is an en-passant square we are able to capture
    // then we first play the move onto the board to check that our king
    // isn't left in check then if our king isn't in check we may add this move
    Bitboard En_Passant_Board = Index_To_Bitboard(game.Current_State.En_Passant);
    if (Move_Board & En_Passant_Board)
    {
      // Create the en-passant moves the only add the move if it doesn't leave our king in check
      En_Passant_Moves = Build_Moves(game, Move_Board & En_Passant_Board, Curr_Index, Our_Pawn, No_Piece);
      En_Passant_Moves = Verify_Moves_Check(game, En_Passant_Moves); // Filters out any moves that leave our king in check

      Capture_Moves.insert(Capture_Moves.end(), En_Passant_Moves.begin(), En_Passant_Moves.end());
     
      Move_Board = Remove_Bit(Move_Board, game.Current_State.En_Passant); // We've finished dealing with the move
    }

    // Add all the normal capture moves
    Capture_Moves = Build_Moves(game, Move_Board, Curr_Index, Our_Pawn, No_Piece);

    Pawn_Board    = Remove_Bit(Pawn_Board, Get_Index(Curr_Pawn)); 
  }
  
  return Validate(game, Moves);
}

vector<Move> Generate_Knight_Moves(const Game &game)
{
  vector<Move> Moves;

  bool     side            = game.Current_State.Side;
  Bitboard Knight_Board    = (side == WHITE ? game.W_Knight : game.B_Knight);
  Bitboard Friendly_Pieces = (game.Current_State.Side == WHITE ? game.W_All : game.B_All);
  Piece    Our_Knight      = (side == WHITE ? White_Knight : Black_Knight);

  // Loop over all knights and generate their legal moves
  Bitboard Curr_Knight, Move_Board; vector<Move> Curr_Moves;
  while (Knight_Board)
  {
    Curr_Knight = Get_LSB(Knight_Board);

    Move_Board  = KNIGHT_MOVES[Get_Index(Curr_Knight)] & (~Friendly_Pieces); // Can't capture your own pieces
    Curr_Moves  = Build_Moves(game, Move_Board, Get_Index(Curr_Knight), Our_Knight, No_Piece);

    Moves.insert(Moves.end(), Curr_Moves.begin(), Curr_Moves.end());
    
    Knight_Board = Remove_Bit(Knight_Board, Get_Index(Curr_Knight));
  }
  
  return Validate(game, Moves);
}

vector<Move> Generate_Bishop_Moves(const Game &game)
{
  Piece        Our_Bishop = (game.Current_State.Side == WHITE ? White_Bishop : Black_Bishop);
  vector<Move> Moves      = Generate_Slider_Moves(game, Our_Bishop);

  return Validate(game, Moves);
}

vector<Move> Generate_Rook_Moves(const Game &game)
{
  Piece        Our_Rook = (game.Current_State.Side == WHITE ? White_Rook : Black_Rook);
  vector<Move> Moves    = Generate_Slider_Moves(game, Our_Rook);

  return Validate(game, Moves);
}

vector<Move> Generate_Queen_Moves(const Game &game)
{
  Piece        Our_Queen = (game.Current_State.Side == WHITE ? White_Queen : Black_Queen);
  vector<Move> Moves     = Generate_Slider_Moves(game, Our_Queen);

  return Validate(game, Moves);
}

vector<Move> Generate_King_Moves(Game &game)
{
  vector<Move> Moves, Curr_Moves;
  
  bool     side            = game.Current_State.Side;
  Bitboard King_Board      = (side == WHITE ? game.W_King : game.B_King);
  Piece    Our_King        = (side == WHITE ? White_King  : Black_King);
  Bitboard Friendly_Pieces = (side == WHITE ? game.W_All  : game.B_All);
  
  // Generate normal moves
  // ----------------------
  Bitboard Move_Board = KING_MOVES[Get_Index(King_Board)] & (~Friendly_Pieces); // Can't capture your own pieces

  Curr_Moves          = Build_Moves(game, Move_Board, Get_Index(King_Board), Our_King, No_Piece);
  Curr_Moves          = Verify_Moves_Check(game, Curr_Moves);

  Moves.insert(Moves.end(), Curr_Moves.begin(), Curr_Moves.end());

  // Generate castling moves
  // ------------------------
  Bitboard Check_Mask = (side == WHITE ? game.White_check_mask : game.Black_check_mask);
  Index    Castle_Sq  = (side == WHITE ? e1 : e8);

  // If king isn't on the castling square or there is a check we may return early because castling will be illegal
  if ((Get_Index(King_Board) != Castle_Sq) || (Check_Mask != NONE)) 
    return Validate(game, Moves);

  // Get relevant castling rights
  bool King_side = (side == WHITE ? 
    game.Current_State.Castle_Rights & W_Kingside :
    game.Current_State.Castle_Rights & B_Kingside); 

  bool Queen_side = (side == WHITE ?
    game.Current_State.Castle_Rights & W_Queenside :
    game.Current_State.Castle_Rights & B_Queenside);

  // We have to make sure that the king doesn't move through check while making this move,
  // and that there are no obstructions in the way of both squares
  Index Right_1 = (side == WHITE ? f1 : f8);
  Index Right_2 = (side == WHITE ? g1 : g8);
  Index Left_1  = (side == WHITE ? d1 : d8);
  Index Left_2  = (side == WHITE ? c1 : c8);
  Index Left_3  = (side == WHITE ? b1 : b8);
  
  // Ensure squares aren't attacked
  if (King_side && (!game.Is_Square_Attacked(Right_1, !side)) && (!game.Is_Square_Attacked(Right_2, !side)))
  {
    // Ensure squares aren't occupied    
    if ((!(game.All_Pieces & Index_To_Bitboard(Right_1))) && (!(game.All_Pieces & Index_To_Bitboard(Right_2))))
      Moves.push_back(Move(Get_Index(King_Board), Right_2, Our_King, false, No_Piece, true, No_Piece, false));
  }

  // Ensure squares aren't attacked
  if (Queen_side && (!game.Is_Square_Attacked(Left_1, !side)) && (!game.Is_Square_Attacked(Left_2, !side)))
  {
    // Ensure squares aren't occupied
    if ((!(game.All_Pieces & Index_To_Bitboard(Left_1))) && (!(game.All_Pieces & Index_To_Bitboard(Left_2))) &&
        (!(game.All_Pieces & Index_To_Bitboard(Left_3))))
      Moves.push_back(Move(Get_Index(King_Board), Left_2, Our_King, false, No_Piece, true, No_Piece, false));
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
