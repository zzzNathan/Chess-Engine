#/*******************************************\
#        M O V E   G E N E R A T I O N       
#        - - - -   - - - - - - - - - -
#\*******************************************/
from Engine.ConstantsAndTables import *
from Engine.Utilities          import *
from typing import Callable

# Generates a list of moves that have target squares as the 1's on a given bitboard
@cache
def Create_Moves_From_Board(Game:GameState,source:i64,bitboard:i64,col:str,piece:str) -> list:
    MoveList    = []
    SourceIndex = GetIndex( source )

    # Get enemy piece bitboard in order to determine whether a move is a capture
    EnemyPieces = Game.WhiteAll if col == 'b' else Game.BlackAll

    # Loop over all precomputed moves and add them to the move list
    while bitboard:
        CurrentBB, TargetIndex = Get_LSB_and_Index( bitboard )

        # Capture moves
        if CurrentBB & EnemyPieces:
            MoveList.append( Move(col,SourceIndex,TargetIndex,True,False,piece,False) )

        # Non-Capture moves
        else: MoveList.append( Move(col,SourceIndex,TargetIndex,False,False,piece,False) )

        # Remove this bit from the board
        bitboard ^= CurrentBB

    return MoveList

# Will generate correct move filter in case piece is pinned or king is in check
@cache
def Generate_Filter(Game:GameState, move:Move) -> i64:
    
    # If this is a king move we dont need to generate a filter
    # It's instead handled inside of the king move generator function
    if move.Piece in ['K','k']: return i64( 2**move.Target )
    
    SourceBB = i64( 2**move.Source )
    TargetBB = i64( 2**move.Target )

    CheckMask = Game.WhiteCheckMask if (move.Side == 'w') else Game.BlackCheckMask
    
    # If there is a check and piece is pinned piece must move to a square where both
    # filters are satisfied
    if SourceBB in Game.Pins: 
        if Is_Check(move.Side, Game) == AllBits: return Game.Pins[ SourceBB ] 
        else: return (Game.Pins[SourceBB] & CheckMask) 

    if Is_Check(move.Side, Game) != AllBits: 
        if move.EnPassant == False: return CheckMask
        
        # Ensure that the pawn being captured is in the check mask
        PassantSquareIdx = Square_to_index[Game.En_Passant]
        if (move.Side == 'w'): PassantSquareIdx -= 8
        else:                PassantSquareIdx += 8
        CapturedPassant = i64(2**PassantSquareIdx)

        return TargetBB if (CapturedPassant & CheckMask) else NoBits

    # If this piece is neither pinned or has a king in check then return the move
    return TargetBB

# Will take in a given move and check if its legal
def Filter_Move(Game:GameState, move:Move) -> Move | bool:
    
    TargetBB = i64( 2**move.Target )
    Filter   = Generate_Filter( Game,move )

    # If the move has passed through the filter return it, otherwise return false
    return move if (TargetBB & Filter) else False
    
# Goes through all moves and ensures that they are all legal
def Filter_All_Moves(Game:GameState, movelist:list[Move]) -> list[Move]:
    FilteredMoveList = []

    for move in movelist:
        if Filter_Move(Game, move):FilteredMoveList.append(move)

    return FilteredMoveList

# Takes in a piece's ascii representation and return a funtion that generates their pseudo-legal moves
@cache
def Find_Move_Table(Game:GameState, char:str) -> Callable:
    char = char.upper()

    # Knights
    if char == 'N': 
        return lambda bitboard : KNIGHT_MOVES[ Board_To_Square(bitboard) ]

    # Bishops
    if char == 'B': 
        return lambda bitboard : Compute_Bishop_attacks(bitboard, Game.AllPieces^bitboard) 
    
    # Rooks
    if char == 'R':
        return lambda bitboard : Compute_Rook_attacks(bitboard,Game.AllPieces^bitboard)
    
    # Queens
    if char == 'Q':
        return lambda bitboard : Compute_Queen_attacks(bitboard,Game.AllPieces^bitboard)

# Take in a given piece bitboard and generate all pseudo-legal moves
# (For all pieces except pawns and king)
@cache
def Gen_Pseudo_legal_Moves(Game:GameState, board:i64, char:str) -> list[Move]:
    MoveList    = []
    colour      = 'w' if (char.isupper()) else 'b'
    FriendlyAll = Game.WhiteAll if (colour == 'w') else Game.BlackAll

    # Gets a function that returns relevant moves for specified piece
    MoveFinder  = Find_Move_Table(Game, char)

    # Iterate through all pieces on the board
    while board:
        Current = Get_LSB(board)
        Target  = MoveFinder(Current)

        # Filter out all moves that capture friendly pieces
        Target ^=  (Target & FriendlyAll)

        MoveList.extend( Create_Moves_From_Board(Game,Current,Target,colour,char) )

        # Remove this bit from the board
        board ^= Current

    return MoveList

@cache
def GeneratePromotions(Source:i64, Target:i64, col:str, Capture:bool) -> list:
    # Getting index of promotion squares
    SourceIndex = int( log2(Source) )
    TargetIndex = int( log2(Target) )

    # Get pawn's piece representation
    piece = 'P' if (col == 'w') else 'p'

    MoveList = []

    MoveList.append(Move(col,SourceIndex,TargetIndex,Capture,False,piece,'n')) # Knight
    MoveList.append(Move(col,SourceIndex,TargetIndex,Capture,False,piece,'b')) # Bishop
    MoveList.append(Move(col,SourceIndex,TargetIndex,Capture,False,piece,'r')) # Rook 
    MoveList.append(Move(col,SourceIndex,TargetIndex,Capture,False,piece,'q')) # Queen

    return MoveList
