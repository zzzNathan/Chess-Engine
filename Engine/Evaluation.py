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

# Will take in an ascii representation of a piece and return its relevant table
def Ascii_To_Table(piece:str, Game:GameState) -> list:
    
    # Map all pieces to their relevant tables
    Map = {
        'P' : Pawn_SQ_TABLE,
        'N' : Knight_SQ_TABLES,
        'B' : Bishop_SQ_TABLES,
        'R' : Rook_SQ_TABLES,
        'Q' : Queen_SQ_TABLES,
        'K' : MergeTables(King_SQ_TABLES_mid, King_SQ_TABLES_end
                          ,Get_GamePhase(Game))
    }

    return Map[ piece.upper() ]

# Takes in an integer from the domain of [0,32] and linearly maps it to a phase score
# of a range [0,1] 
def Normalise(n:int) -> float: return (n-2)/30

# Function to get the current game phase, a game phase of 0 means we are in the early game while 
# a game phase of 1 means that we are late into the endgame
def Get_GamePhase(Game:GameState) -> float: 

    # We will use the number of pieces left on the board to deterimine the game phase
    # This implies that 32 pieces will garner a game phase score of 0
    # and 2 pieces will garner a game phase score of 1

    pieces = 0 # Variable to track number of pieces
    for board in Game.Pieces: pieces += BitCount( board )

    return Normalise(pieces)

# Function to linealy interpolate between a middle game table and and endgame table
# depending on the given "Phase" score
def MergeTables(TableMid:list, TableEnd:list, Phase:float) -> list: 
    
    # Initialise the merged table
    Table = [[-1 for i in range(8)] for j in range(8)]
    # Iterate over all the squares in both tables
    for row in range(8):
        for col in range(8):

            # Get the value of the relevant square in both the midgame and engame tables
            MidSquare = TableMid[row][col]
            EndSquare = TableEnd[row][col]
            
            # Calculate the weighted sum of both squares and put it into the table
            WeightedSquare  = ( EndSquare * Phase ) + ( MidSquare * (1-Phase) )
            Table[row][col] = WeightedSquare
            
    return Table 

# Iterate through a given bitboard and use a given piece square table to properly count
# centipawn value with biases for positon
def Add_Weighted_Material(Board:i64, Table:list) -> int:
    score = 0

    while Board:
        
        # Get lowest 1 bit on the board and its relevant index
        current, index = Get_LSB_and_Index(Board)
        
        score += Table[index]

        # Remove this bit from the board
        Board ^= current

    return score

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
        NotImplemented

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

    # Who has more legal moves ?
    Score += (WhiteMobility(Game) - BlackMobility(Game))

    return Score

# IN PROGRESS
print( Evaluate(STARTING_GAME) )
