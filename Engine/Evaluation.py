#/*******************************************\
#            E V A L U A T I O N S  
#            - - - - - - - - - - - 
#\*******************************************/
from Engine.ConstantsAndTables import *
from Engine.Utilities          import *
from Engine.MoveGenerator      import *
from Engine.PieceSquareTables  import *

# References:
# ----------
# https://www.chessprogramming.org/Simplified_Evaluation_Function
# https://chess.stackexchange.com/questions/17957/how-to-write-a-chess-evaluation-function

INF          = 1_000_000
PAWN_VALUE   = 100
KNIGHT_VALUE = 320
BISHOP_VALUE = 330
ROOK_VALUE   = 500
QUEEN_VALUE  = 900
KING_VALUE   = 20_000

# Values to increase or decrease evaluation scores based on their importance to the game
DAMPING = {}

# Computes the amount of material the given side has
def Material(Game:GameState, col:str) -> int:
    Score = 0
    Boards = Game.WhitePieces if (col == 'w') else Game.BlackPieces

    for board in Boards:
        name   = Board_To_Ascii(Game, board)
        Score += Add_Weighted_Material(board, Ascii_To_Table(name,Game), col=='b')

    return Score

# Computes how many legal moves each colour has
def Mobility(Game:GameState, col:str) -> float:
    return len(Generate_Moves(Game, col))

# Gives bonuses based on how many pawns are connected
def Connectivity(Game:GameState, col:str) -> float:
    
    # Get bitboard of white pawns then AND it with the board shifted left then right (with masking)
    # Then all bits remaining in the and will be connected
    Board             = Game.WhitePawn if (col=='w') else Game.BlackPawn
    Board_Shift_Left  = (Board << i64(1)) & NotFileH
    Board_Shift_Right = (Board >> i64(1)) & NotFileA

    Board_Shift_Left  &= Board
    Board_Shift_Right &= Board
   
    return BitCount( Board_Shift_Left | Board_Shift_Right )

# Gives a small bonus if this side has the bishop pair
def BishopPair(Game:GameState, col:str) -> float:
    Bishops = Game.WhiteBishop if (col == 'w') else Game.BlackBishop
    return 7.5 if (BitCount(Bishops) >= 2) else 0.0

# Gives a penalty if this side has doubled pawns
def DoubledPawns(Game:GameState, col:str) -> float:

    # Get bitboard of given coloured pawns then AND it with the same board shifted up by one
    # Then all the remaining bits will be doubled pawns
    Board          = Game.WhitePawn if (col=='w') else Game.BlackPawn
    Board_Shift_Up = Board << i64(8)
    
    # Multiplies score by -1 for white because this is a penalty
    return BitCount( Board & Board_Shift_Up ) * [1,-1][col == 'w']

# Bonuses for passed pawns 
def PassedPawns(Game:GameState, col:str) -> float:
    # Bonuses to be given for how far away the passed pawn is from promoting
    PassedPawnsBonuses = [0, 90, 60, 40, 25, 15, 15]
    Score = 0
    Pawns = Game.WhitePawn if (col == 'w') else Game.BlackPawn

    # Iterate through all pawns on the board and calculate their passed pawn bonuses
    while Pawns:
        CurrentPawn, CurrentIndex = Get_LSB_and_Index(Pawns)

        # Get square that will be at the end of the board
        if (col == 'w'):
            EndOfBoard = CurrentIndex + 8 * ((63-CurrentIndex)//8)
            EndOfBoard = i64(2**EndOfBoard)
        
        else:
            EndOfBoard = CurrentIndex - 8 * (CurrentIndex//8)
            EndOfBoard = i64(2**EndOfBoard)

        PawnUpOne = CurrentPawn << i64(8) if (col == 'w') else CurrentPawn >> i64(8) 

        # Get squares in front of the pawn one to the right and one to the left 
        mask  = Build_Ray(PawnUpOne, EndOfBoard)
        mask &= ((mask >> i64(1)) & NotFileA) | ((mask << i64(1)) & NotFileH) 

        # If no pawns in front or 1 right or 1 left this is a passed pawn
        if (mask & Game.AllPieces == NoBits):
            Score += PassedPawnsBonuses[ BitCount(Build_Ray(PawnUpOne, EndOfBoard)) ]

        # Remove this bit from the board
        Pawns ^= CurrentPawn

    return Score

# Bonuses for rooks on open files
def OpenRookFiles(Game:GameState, col:str) -> float:
    Score = 0.0
    Rooks = Game.WhiteRook if (col == 'w') else Game.BlackRook
    
    # Iterate through all rooks on the board and calculate their open file bonuses
    while Rooks:
        CurrentRook = Get_LSB(Rooks)

        # Find what file the rook is on 
        CurrentFile = i64(0)
        for file in FILES:
            if CurrentRook & file:
                CurrentFile = file
                break
        
        # Calculates how many squares are open that the rook can see
        Score += 8 - BitCount(Compute_Rook_attacks(CurrentRook, Game.AllPieces) & CurrentFile & Game.AllPieces)
        
        # Remove this bit from the board
        Rooks ^= CurrentRook

    return Score

# Evaluates the current position
def Evaluate(Game:GameState) -> float:
    Score = 0

    # Check if white of black has checkmate
    if Is_Check('w',Game)!=AllBits and Generate_White_King_Moves(Game.WhiteKing,Game) == []: return -INF
    if Is_Check('b',Game)!=AllBits and Generate_Black_King_Moves(Game.BlackKing,Game) == []: return INF

    Score += Material(Game, 'w')     - Material(Game, 'b')
    Score += Mobility(Game, 'w')     - Mobility(Game, 'b')
    Score += Connectivity(Game, 'w') - Connectivity(Game, 'b')
    Score += DoubledPawns(Game, 'w') - DoubledPawns(Game, 'b')
    Score += OpenRookFiles(Game,'w') - OpenRookFiles(Game,'b')
    Score += PassedPawns(Game ,'w')  - PassedPawns(Game, 'b')
    return Score

# Should be favourable for white // TESTS
fen = r'rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8'  
GAME = Fen_to_GameState(fen)
print(Evaluate(GAME))
print( Show_Board(GAME) )
