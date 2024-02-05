#/*******************************************\
#      U T I L I T Y  F U N C T I O N S  
#      - - - - - - -  - - - - - - - - -
#\*******************************************/
import numpy as np
from ConstantsAndTables import *
from ClassUtilities     import *
from PieceSquareTables  import *
from BitMacros          import *

# Shorthands
i64 = np.uint64
i8  = np.uint8  

# Helper dictionary that maps piece ascii codes to their human readable names
Ascii_To_Name = {'P':'WhitePawn', 'p':'BlackPawn', 'N':'WhiteKnight', 'n':'BlackKnight',
                 'B':'WhiteBishop', 'b':'BlackBishop', 'R':'WhiteRook', 'r':'BlackRook',
                 'Q':'WhiteQueen', 'q':'BlackQueen', 'K':'WhiteKing', 'k':'BlackKing'}

# Presents the board with all bitboards combined
def Show_Board(Game:GameState) -> str:
    Board = []
    # Iterates over board ranks
    for rank in range(7,-1,-1):
        result = ''

        # Iterates over board files
        for file in range(7,-1,-1):

            # Calculates square number
            SquareNum = rank * 8 + file

            # Iterates over all piece bitboards
            for piece in Game.Pieces:

                # If this bit is set add the piece's ascii representation
                if Get_bit( piece,SquareNum ):

                    result = f'{ Board_To_Ascii(Game,piece) } {result}'
                    break

            # Add a dot to show an empty square
            else: result = f'. {result}'

        # Add this rank to the board with the number on the beginning
        result = f'{rank+1}| {result}'

        # Add to Board
        Board.append(result)

    # Adds the letters on the bottom
    Board.append('   ---------------')
    Board.append('   a b c d e f g h')
    # Return with new lines on each rank
    return '\n'.join(Board)
  
# Returns true if square is attacked by given colour
def Is_square_attacked(SquareNum:int|i64, colour:str, Game:GameState) -> bool:
    # If a bitboard was passed in then change it to a square 
    if type( SquareNum ) == i64: SquareNum = GetIndex( SquareNum )

    bitboard = i64(2**SquareNum)

    # Get relevant piece bitboards
    Attacking_King     = Game.WhiteKing   if (colour == 'w') else Game.BlackKing
    Attacking_Pawn     = Game.WhitePawn   if (colour == 'w') else Game.BlackPawn
    Attacking_Knight   = Game.WhiteKnight if (colour == 'w') else Game.BlackKnight
    Attacking_Bishop   = Game.WhiteBishop if (colour == 'w') else Game.BlackBishop
    Attacking_Rook     = Game.WhiteRook   if (colour == 'w') else Game.BlackRook
    Attacking_Queen    = Game.WhiteQueen  if (colour == 'w') else Game.BlackQueen
    Opposite_Pawn_Atks = BLACK_PAWN_ATKS[SquareNum] if (colour == 'w') else WHITE_PAWN_ATKS[SquareNum]

    # Does a king attack this square?
    if KING_MOVES[SquareNum] & Attacking_King: return True 

    # Does a pawn attack this square?
    if Opposite_Pawn_Atks & Attacking_Pawn: return True

    # Does a knight attack this square?
    if KNIGHT_MOVES[SquareNum] & Attacking_Knight: return True

    # Does a bishop attack this square?
    if Compute_Bishop_attacks( bitboard, Game.AllPieces ) & Attacking_Bishop: return True

    # Does a rook attack this square?
    if Compute_Rook_attacks( bitboard, Game.AllPieces ) & Attacking_Rook: return True

    # Does a queen attack this square?
    if Compute_Queen_attacks( bitboard, Game.AllPieces ) & Attacking_Queen: return True

    # Return false if no pieces attack this square
    return False

# Returns a bitboard with all of the squares that the given colour attacks
def All_Attacked_squares(col:str, Game:GameState) -> i64:
    bitboard = i64(0)
    # Loop over all squares
    for square in range(0,64):

        # If we attack this square add it to the bitboard
        if Is_square_attacked(square, col, Game): bitboard = Set_bit(bitboard, square)

    return bitboard        

# Takes in an ascii representation and returns relevant bitboard
def Ascii_To_Board(Game:GameState, piece:str) -> i64:
    # Map each letter to relevant board
    Char_To_Board = {
        'P':Game.WhitePawn, 'p':Game.BlackPawn, 'N':Game.WhiteKnight, 'n':Game.BlackKnight,
        'B':Game.WhiteBishop, 'b':Game.BlackBishop, 'R':Game.WhiteRook, 'r':Game.BlackRook,
        'Q':Game.WhiteQueen, 'q':Game.BlackQueen, 'K':Game.WhiteKing, 'k':Game.BlackKing }

    # Return the board that was required
    return Char_To_Board[ piece ]

# Takes in bitboard and returns the relevant ascii representation
def Board_To_Ascii(Game:GameState, board:i64) -> str:

    # Map each board to relevant letter
    Board_To_Char = {
        Game.WhitePawn: 'P', Game.BlackPawn: 'p', Game.WhiteKnight: 'N', Game.BlackKnight: 'n',
        Game.WhiteBishop: 'B', Game.BlackBishop: 'b', Game.WhiteRook: 'R', Game.BlackRook: 'r', 
        Game.WhiteQueen: 'Q', Game.BlackQueen: 'q', Game.WhiteKing:'K', Game.BlackKing :'k' }
    
    # Return the character required
    return Board_To_Char[ board ]

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

# Takes in an integer from the domain of [0,32] and linearly maps it to a phase score of a range [0,1] 
def Normalise(n:int) -> float: return (n-2)/30

# Function to get the current game phase, a game phase of 0 means we are in the early game while 
# a game phase of 1 means that we are late into the endgame
def Get_GamePhase(Game:GameState) -> float: 

    # Counts up how many pieces are on the board
    pieces = 0 
    for board in Game.Pieces: pieces += BitCount( board )
    
    # Linearly maps the number of pieces into a range of [0,1] to be used as the "phase score"
    return Normalise(pieces)

# Function to linealy interpolate between a middle game table and and endgame table
# depending on the given "Phase" score
def MergeTables(TableMid:list, TableEnd:list, Phase:float) -> list: 
    
    # Initialise the merged table
    Table = [None for _ in range(64)]

    for square in range(64):

        # Get the value of the relevant square in both the middle and endgame tables
        MidSquare = TableMid[square]
        EndSquare = TableEnd[square]

        # Calculate the weighted sum of both squares and put it into the table
        WeightedSquare  = ( EndSquare * Phase ) + ( MidSquare * (1-Phase) )
        Table[square] = WeightedSquare
   
    return Table 

# Iterate through a given bitboard and use a given piece square table to properly count
# centipawn value with biases for positon
def Add_Weighted_Material(Board:i64, Table:list, Black=False) -> int:
    score = 0
    
    # If this is a black piece we must remap the square to where it would be when viewing the board
    # from white's perspective
    def Remap(square:int) -> int:
        if Black: return Black_To_White[square]
        else: return square

    while Board:
        
        # Get lowest 1 bit on the board and its relevant index
        current, index = Get_LSB_and_Index(Board)
        
        # Add relevant weighting to the score
        score += Table[ Remap(index) ]

        # Remove this bit from the board
        Board ^= current

    return score
