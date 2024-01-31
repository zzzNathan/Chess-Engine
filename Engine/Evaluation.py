#/*******************************************\
#            E V A L U A T I O N S  
#            - - - - - - - - - - - 
#\*******************************************/

from ConstantsAndTables import *
from Utilities          import *
from MoveGenerator      import *
from PieceSquareTables  import *

INF = 20_000

# Centipawn piece values 
# (values from Claude Shannon's MIT paper "Programming a computer to play chess")
WEIGHTS = { 
'K' : INF, # King weight
'Q' : 900, # Queen weight
'N' : 320, # Knight weight
'B' : 330, # Bishop weight
'R' : 500, # Rook weight
'P' : 100, # Pawn weight
}

# Values to increase or decrease evaluation scores based on their importance to the game
Damping = {}

# Heuristic to determine centipawn value of white's material 
def Material(Game:GameState, col:str) -> int:
    Score = 0

    Boards = Game.WhitePieces if (col=='w') else Game.BlackPieces
    for board in Boards:

        # Get piece name 
        name = Board_To_Ascii( Game,board )

        # Iterate through this board and add the relevant centipawn value to score
        Score += Add_Weighted_Material( board,Ascii_To_Table(name,Game),col=='b' )

    return Score

# Gives bonus based on how mnay legal move the given colour can make
def Mobility(Game:GameState, col:str) -> int:

    # Return the number of legal moves for given colour
    return len( Generate_Moves(Game, col) )

# Gives bonus based on how many pawns are connected
def Connectivity(Game:GameState, col:str) -> int:
    
    # Get bitboard of white pawns then AND it with the board shifted left then right (with masking)
    # Then all bits remaining in the and will be connected
    Board             = Game.WhitePawn if (col=='w') else Game.BlackPawn
    Board_Shift_Left  = (Board << i64(1)) & NotFileH
    Board_Shift_Right = (Board >> i64(1)) & NotFileA

    Board_Shift_Left  &= Board
    Board_Shift_Right &= Board
    
    return BitCount( Board_Shift_Left | Board_Shift_Right )

# Gives penalty based on how many pawns are doubled
def DoubledPawns(Game:GameState, col:str) -> int:

    # Get bitboard of given coloured pawns then AND it with the same board shifted up by one
    # Then all the remaining bits will be doubled pawns
    Board          = Game.WhitePawn if (col=='w') else Game.BlackPawn
    Board_Shift_Up = Board << i64(8)

    return BitCount( Board & Board_Shift_Up ) 

'''
Mobility bonus 0.1
Outpost bonus  
Attack bonus
Rook on the seventh rank
Material advantage
Minor pieces behind pawn
'''

'''
Basic Evaluation Features

Pawn Structure
Evaluation of Pieces
Evaluation Patterns
Center Control
Trapped Pieces
King Safety
Space
'''

# Returns the evaluation of a position, (how likely white or black is to win)
#       ( +ve sign signifies an advantage for white )
#       ( -ve sign signifies an advantage for black )
def Evaluate(Game:GameState) -> int:
    Score = 0
    
    # Who has more material ?
    Score += (Material(Game,'w') - Material(Game,'b')) #*Damping['Material']

    # Who has more legal moves ?
    Score += (Mobility(Game,'w') - Mobility(Game,'b')) #* Damping['Mobility']

    # Who has more connected pawns ?
    Score += (Connectivity(Game,'w') - Connectivity(Game,'b')) #* Damping['Connectivity']
   
    # Who has less doubled pawns ?
    Score += (DoubledPawns(Game, 'b') - DoubledPawns(Game, 'w')) #* Damping['DoubledPawns']
    
    return Score

# IN PROGRESS
print( Evaluate(STARTING_GAME) )
