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
        Score += Add_Weighted_Material( board,Ascii_To_Table(name,Game) )

    return Score

# Determines how many squares the given colour controls
def Mobility(Game:GameState, col:str) -> int:

    # Return the number of legal moves for given colour
    return len( Generate_Moves(Game, col) )

# Determines whether the pawn structure is connected 
def Connectivity(Game:GameState, col:str) -> int:
    
    # Get bitboard of white pawns then and it with the board shifted left then right (with masking)
    # Then all bits remaining in the and will be connected
    Board             = Game.WhitePawn if (col=='w') else Game.BlackPawn
    Board_Shift_Left  = (Board << i64(1)) & NotFileH
    Board_Shift_Right = (Board >> i64(1)) & NotFileA

    Board_Shift_Left  &= Board
    Board_Shift_Right &= Board
    
    # Xor is used to ensure that the same pawn isn't double counted
    return BitCount( Board_Shift_Left ^ Board_Shift_Right )


# For double pawns and the biboard with itself shifted up by one and the remaining bits will all be doubles

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
 
    return Score

# IN PROGRESS
print( Evaluate(STARTING_GAME) )
