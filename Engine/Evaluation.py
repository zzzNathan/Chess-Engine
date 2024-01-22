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

# Heuristic to determine centipawn value of white's material 
def WhiteMaterial(Game:GameState) -> int:
    Score = 0
    for board in Game.WhitePieces:

        # Get piece name 
        name = Board_To_Ascii( Game,board )

        # Iterate through this board and add the relevant centipawn value to score
        Score += Add_Weighted_Material( board,Ascii_To_Table(name,Game) )

    return Score

# Heuristic to determine centipawn value of black's material
def BlackMaterial(Game:GameState) -> int:
    Score = 0
    for board in Game.BlackPieces:
        
        # Get piece name
        name = Board_To_Ascii( Game,board )

        # Iterate through this board and add the relevant centipawn value to score
        Score += Add_Weighted_Material( board,Ascii_To_Table(name,Game),True )

    return Score

# Determines how many squares white controls
def WhiteMobility(Game:GameState) -> int:

    # Return the number of legal moves for white
    return len( Generate_Moves(Game,'w') )

# Determines how many squares black controls
def BlackMobility(Game:GameState) -> int:

    # Return the number of legal moves for white
    return len( Generate_Moves(Game,'b') )

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

Piece-Square Tables
Pawn Structure
Evaluation of Pieces
Evaluation Patterns
Center Control
Connectivity
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
    Score += (WhiteMaterial(Game) - BlackMaterial(Game)) 

    # Who has more legal moves ? black has one more legal move
    Score += (WhiteMobility(Game) - BlackMobility(Game))

    return Score

# IN PROGRESS
print( Evaluate(STARTING_GAME) )
