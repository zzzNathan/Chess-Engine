#/*******************************************\
#      U T I L I T Y  F U N C T I O N S  
#      - - - - - - -  - - - - - - - - -
#\*******************************************/

# Chess engine in Python
from textwrap import wrap
import numpy as np
from ConstantsAndTables import *

# Shorthands
i64 = np.uint64
i8  = np.uint8

# Macros
# -------------------------------------------------------------------------------

# Returns 0 if a bitboard hasn't got this bit as a one
def Get_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard & i64(2**SquareNum)

# Sets a bit on bitboard, 0 to 1
def Set_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard | i64(2**SquareNum)

# Removes a bit from bitboard, 1 to 0
def Delete_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard ^ i64(2**SquareNum)

# Gets the least significant bit (bit worth the lowest value)
def Get_LSB(Bitboard:i64) -> i64:
    if Bitboard:return Bitboard & (~Bitboard + i64(1))
    # No bits on bitboard so return -1
    return -1

# Checks if given bitboard resides on the 2nd rank (used to validate pawn moves)
def Is_Second_Rank(bitboard:i64) -> bool: return bitboard & Rank2

# Checks if given bitboard resides on the 1st rank (used to validate pawn moves)
def Is_First_Rank(bitboard:i64) -> bool: return bitboard & Rank1

# Checks if given bitboard resides on the 7th rank (used to validate pawn moves)
def Is_Seventh_Rank(bitboard:i64) -> bool: return bitboard & Rank7

# Checks if given bitboard resides on the 8th rank (used to validate pawn moves)
def Is_Eigth_Rank(bitboard:i64) -> bool: return bitboard & Rank8

# Returns both the least significant bit and the index from (0 - 63)
def Get_LSB_and_Index(bitboard:i64) -> tuple[i64,int]:
    LSB = Get_LSB(bitboard)
    return LSB, int( math.log2(LSB) )

# Returns index of the one bit on the given bitboard
def GetIndex(bitboard:i64) -> int: return int( math.log2(bitboard) )

# Checks whether the given board has bits within the mask
def In_Mask(bitboard, mask): return bitboard & mask 

# Function to count how many bits are in some binary representation of a number
def BitCount(n:i64) -> int:
    count = 0
    while n:
        # Removes last bit
        n &= ( n-i64(1) )

        # Increment count by one until no more one bits left
        count += 1

    return count

# Helper functions
# -------------------------------------------------------------------------------

# Dictionary that takes string of a square's name and gives its index ('e4') -> 36
Square_to_index = {
    'h1': 0,  'g1': 1,  'f1': 2,  'e1': 3,  'd1': 4,  'c1': 5,  'b1': 6,  'a1': 7, 
    'h2': 8,  'g2': 9,  'f2': 10, 'e2': 11, 'd2': 12, 'c2': 13, 'b2': 14, 'a2': 15, 
    'h3': 16, 'g3': 17, 'f3': 18, 'e3': 19, 'd3': 20, 'c3': 21, 'b3': 22, 'a3': 23, 
    'h4': 24, 'g4': 25, 'f4': 26, 'e4': 27, 'd4': 28, 'c4': 29, 'b4': 30, 'a4': 31, 
    'h5': 32, 'g5': 33, 'f5': 34, 'e5': 35, 'd5': 36, 'c5': 37, 'b5': 38, 'a5': 39, 
    'h6': 40, 'g6': 41, 'f6': 42, 'e6': 43, 'd6': 44, 'c6': 45, 'b6': 46, 'a6': 47, 
    'h7': 48, 'g7': 49, 'f7': 50, 'e7': 51, 'd7': 52, 'c7': 53, 'b7': 54, 'a7': 55, 
    'h8': 56, 'g8': 57, 'f8': 58, 'e8': 59, 'd8': 60, 'c8': 61, 'b8': 62, 'a8': 63}

# Inverse of Sqaure to index, take an index and return the square's name 36 -> ('e4')
Index_to_square = {square:index for index,square in Square_to_index.items()}

# Builds an attacking ray between 2 squares if the 2 squares are in line 
# horizontally, vertically or diagonally (Used in Is_Check function)
def Build_Ray(Square1:i64, Square2:i64) -> i64:
    # Empty board for generating ray attacks without obstructions
    Empty = i64(0)

    # Test for diagonal rays
    Ray = Compute_Bishop_attacks( Square1,Empty ) & Compute_Bishop_attacks( Square2,Empty )

    if Ray: return Ray ^ (Square1 | Square2) # Adds both target squares to ray

    # If ray isnt diagonal it must be vertical or horizontal
    else: return ( ( Compute_Rook_attacks( Square1,Empty )# Adds in both target squares to ray
                   & Compute_Rook_attacks( Square2,Empty ) ) ^ (Square1 | Square2) )

# Helper dictionary that maps piece ascii codes to their human readable names
Ascii_To_Name = {'P':'WhitePawn', 'p':'BlackPawn', 'N':'WhiteKnight', 'n':'BlackKnight',
                 'B':'WhiteBishop', 'b':'BlackBishop', 'R':'WhiteRook', 'r':'BlackRook',
                 'Q':'WhiteQueen', 'q':'BlackQueen', 'K':'WhiteKing', 'k':'BlackKing'}

# Classes
# -------------------------------------------------------------------------------

# Class to give structure and order to moves and other relevant information
class Move():

    def __init__(self,side,source,target,capture,castle,piece,promo ) -> None:
        # 'w' signifies white, 'b' signifies black
        self.Side       = side
        # The index of the source square
        self.Source     = source
        # The index of the target square
        self.Target     = target
        # Is this move a capture ?
        self.Capture    = capture
        # Is this move a castle ?
        self.Castle     = castle
        # Capital refers to a white piece, lowercase refers to a black piece
        # Standard chess notation applies
        self.Piece      = piece
        # Is this move a promotion ?
        self.Promotion  = promo
        
# Class for storing all gamestate variables in one place
class GameState():

    # Initialises bitboards representing the game
    def InitBoards(self):
        # White pieces
        self.WhitePawn   = i64(0)
        self.WhiteKnight = i64(0)
        self.WhiteBishop = i64(0)
        self.WhiteRook   = i64(0)
        self.WhiteQueen  = i64(0)
        self.WhiteKing   = i64(0)
        # Black pieces
        self.BlackPawn   = i64(0)
        self.BlackKnight = i64(0)
        self.BlackBishop = i64(0)
        self.BlackRook   = i64(0)
        self.BlackQueen  = i64(0)
        self.BlackKing   = i64(0)
        # Piece combinations
        self.WhiteAll    = i64(0)
        self.BlackAll    = i64(0)
        self.AllPieces   = i64(0)
        # Piece iterators
        self.WhitePieces = [self.WhitePawn, self.WhiteKnight, self.WhiteBishop, self.WhiteRook, self.WhiteQueen, self.WhiteKing]
        self.BlackPieces = [self.BlackPawn, self.BlackKnight, self.BlackBishop, self.BlackRook, self.BlackQueen, self.BlackKing]
        self.Pieces      = [*self.WhitePieces, *self.BlackPieces]

    # Initialising all attributes for the game
    def __init__(self,side:str,Pos:str,EnPassant:str,CastleRights:i8,HalfMove:int,FullMove:int):
        # Either 'w' or 'b' , signifying white or black respectively
        self.Side_To_Move    = side

        # Fen string with normal FEN-notation https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
        self.FEN             = Pos

        # Stored as a string of the referenced square (e.g. 'e4')
        self.En_Passant      = EnPassant

        # 4-bit binary number (First 2 bits are white's queenside and kingside castle rights respectively)
        #                     (Last 2 bits are black's queenside and kingside castle rights respectively) 
        self.Castle_Rights   = CastleRights

        # The number of halfmoves since the last capture or pawn advance, used for the fifty-move rule.
        self.Half_Move_Clock = HalfMove

        #The number of the full moves. It starts at 1 and is incremented after Black's move
        self.Full_Move_Clock = FullMove

        # A mask containing the attack rays of any checks on the white king
        self.WhiteCheckMask  = False

        # A mask containing the attack rays of any checks on the black king
        self.BlackCheckMask = False

        # A dictionary mapping bitboard squares to their relevant attack rays if this piece is pinned
        # (Pinned pieces can only move along the attack ray)
        self.Pins           = {}

        self.InitBoards()
        self.Parse_FEN( Pos )

    # Takes an ascii representation of a piece and returns relevant bitboard
    def Ascii_To_Board(self,char):

        # Map each letter to relevant board
        Char_To_Board = {'P':self.WhitePawn, 'p':self.BlackPawn, 'N':self.WhiteKnight, 'n':self.BlackKnight,
                         'B':self.WhiteBishop, 'b':self.BlackBishop, 'R':self.WhiteRook, 'r':self.BlackRook,
                         'Q':self.WhiteQueen, 'q':self.BlackQueen, 'K':self.WhiteKing, 'k':self.BlackKing}

        # Return the board that was required
        return Char_To_Board[ char ]
    
    # Takes a game board and returns the relevant ascii representation of that board
    def Board_To_Ascii(self,board):

        # Map each board to relevant letter
        Board_To_Char = {self.WhitePawn: 'P', self.BlackPawn: 'p', self.WhiteKnight: 'N', self.BlackKnight: 'n',
                         self.WhiteBishop: 'B', self.BlackBishop: 'b', self.WhiteRook: 'R', self.BlackRook: 'r', 
                         self.WhiteQueen: 'Q', self.BlackQueen: 'q', self.WhiteKing:'K', self.BlackKing :'k'}
        
        return Board_To_Char[ board ]

    # Takes a Forsyth-edwards notation string and prints the relevant game state
    def Parse_FEN(self,fen:str):
        # Separate fen into separate strings based on what information they have
        fen = fen.split()
        
        # Iterates through each rank of the board
        Square = 63
        for rank in fen[0].split('/'):
            
            # Iterate through pieces on each rank
            for piece in rank:

                # If there is a number in the fen, this indicates the number of empty squares found
                if piece.isdigit(): Square -= int( piece )

                # Otherwise place a piece on the relevant bitboard square
                else: 
                    # Update bitboard with new piece
                    Board  = self.Ascii_To_Board( piece )
                    Board |= i64( 2**Square )
                    Name   = Ascii_To_Name[ piece ] 

                    # Update actual board attribute
                    setattr( self,Name,Board )
                    Square -= 1

        # Update gamestate attributes
        self.Side_To_Move = fen[1]
        self.Castle_Rights = i8(0)
        if fen[2] != '-':
            for castle in fen[2]:
                match castle:
                    # Castles are encoded as following: - White controls the 4 and 8 bits
                    #                                   - Black controls the 1 and 2 bits
                    # Kingside is the rightmost bit, Queenside is the leftmost bit                                 
                    case 'K': self.Castle_Rights != i8(0b0100)
                    case 'Q': self.Castle_Rights |= i8(0b1000)
                    case 'k': self.Castle_Rights |= i8(0b0001)
                    case 'q': self.Castle_Rights |= i8(0b0010)
        self.En_Passant = None if fen[3] == '-' else fen[3]
        self.Half_Move_Clock = fen[4]
        self.Full_Move_Clock = fen[5]

    # Plays a given move onto the board
    def Make_Move(self,move:Move):

        # Get side to move
        Colour = move.Side

        # Gets correct piece representation
        Piece = move.Piece

        # Get source and target bitboards
        Source = i64( 2**move.Source )
        Target = i64( 2**move.Target )
        
        # Handle castling moves
        if move.Castle == True: Make_Castle( self,Source,Target,Colour )

        # Handle captures
        if move.Capture == True: Make_Capture( self,Target,Colour )

        # Update actual bitboard
        Board  = self.Ascii_To_Board( Piece )
        Board ^= ( Source | Target )

        # Update the attribute to the new bitboard
        Name = Ascii_To_Name[ Piece ]
        setattr( self,Name,Board )
            
        # Update occupancies
        self.WhiteAll  = (self.WhitePawn | self.WhiteKnight | self.WhiteBishop | self.WhiteRook | self.WhiteQueen | self.WhiteKing)
        self.BlackAll  = (self.BlackPawn | self.BlackKnight | self.BlackBishop | self.BlackRook | self.BlackQueen | self.BlackKing)
        self.AllPieces = ( self.WhiteAll | self.BlackAll )

# Move Generation helper functions
# -------------------------------------------------------------------------------

# Checks whether the given square bitboard is obstructed by any piece
def Is_Obstructed(Game:GameState,bitboard:i64): return Game.AllPieces & bitboard

# Nice way to visualise the board
def Show_bitboard(bb:int) -> str:
    # Fills the binary with extra zeros until 64 digits, (8x8 board)  
    result = str(bin(bb)[2:]).zfill(64)
    print(result)               
    return '\n'.join([' '.join(wrap(line, 1)) for line in wrap(result, 8)])

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

                    result = f'{ Game.Board_To_Ascii(piece) } {result}'
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
def Is_square_attacked(SquareNum:int, colour:str, Game:GameState) -> bool:
    bitboard = i64(2**SquareNum)
    # Does a white piece attack this square?
    if colour == 'w':
        # Does a white king attack this square?
        if KING_MOVES[SquareNum] & Game.WhiteKing: return True 

        # Does a white pawn attack this square?
        if BLACK_PAWN_ATKS[SquareNum] & Game.WhitePawn: return True

        # Does a white knight attack this square?
        if KNIGHT_MOVES[SquareNum] & Game.WhiteKnight: return True

        # Does a white bishop attack this square?
        if Compute_Bishop_attacks( bitboard, Game.AllPieces ) & Game.WhiteBishop: return True

        # Does a white rook attack this square?
        if Compute_Rook_attacks( bitboard, Game.AllPieces ) & Game.WhiteRook: return True

        # Does a white queen attack this square?
        if Compute_Queen_attacks( bitboard, Game.AllPieces ) & Game.WhiteQueen: return True
    else:
        # Does a black king attack this square?
        if KING_MOVES[SquareNum] & Game.BlackKing: return True

        # Does a black pawn attack this square?
        if WHITE_PAWN_ATKS[SquareNum] & Game.BlackPawn: return True

        # Does a black knight attack this square?
        if KNIGHT_MOVES[SquareNum] & Game.BlackKnight: return True

        # Does a black bishop attack this square?
        if Compute_Bishop_attacks( bitboard, Game.AllPieces ) & Game.BlackBishop: return True

        # Does a black rook attack this square?
        if Compute_Rook_attacks( bitboard, Game.AllPieces ) & Game.BlackRook: return True

        # Does a black queen attack this square?
        if Compute_Queen_attacks( bitboard, Game.AllPieces ) & Game.BlackQueen: return True
    # Return False if no pieces attack this square
    return False

# Returns a bitboard with all of the squares that the given colour attacks
def All_Attacked_squares(col:str, Game:GameState) -> i64:
    bitboard = i64(0)
    # Loop over all squares
    for square in range(0,64):

        # If we attack this square add it to the bitboard
        if Is_square_attacked(square, col, Game): bitboard = Set_bit(bitboard, square)

    return bitboard        

# Plays a castling move onto the game board
def Make_Castle(Game:GameState, Source:i64, Target:i64, Colour:str):
    # Get correct king bitboard
    King = Game.WhiteKing if Colour == 'w' else Game.BlackKing

    # Get correct rook bitboard
    Rook = Game.WhiteRook if Colour == 'w' else Game.BlackRook

    # True if the king moves to the right
    Right = True if Target < Source else False

    # Move king
    King ^= ( Source | Target )

    # Move rook
    if Right:
        if Colour == 'w': Rook ^= ( i64(1) | i64(4) )
        else: Rook ^= ( i64(2**56) | i64(2**58) )
    else:
        if Colour == 'w': Rook ^= ( i64(2**7) | i64(2**4) )
        else: Rook ^= ( i64(2**63) | i64(2**60) )

# Plays a capture onto the game board
def Make_Capture(Game:GameState, Target:i64, Colour:str):
    # Iterate through enemy bitboards until we find the correct one
    for EnemyPiece in (Game.BlackPieces if Colour == 'w' else Game.WhitePieces):

        # If this enemy bitboard contains the piece we captured
        if Remove := (EnemyPiece & Target): 

            # Remove this piece from the board
            EnemyPiece ^= Remove
            break

# Determines whether there is a check or not if so returns a bitboard of the attacking ray
# (Used in legal move generation)
def Is_Check(col:str, Game:GameState):
    # Will store rays or locations of checking pieces
    masks = []

    # Shorthand for colour checks 
    Colour = True if col == 'w' else False

    # Select correct king bitboard
    King = Game.WhiteKing if Colour else Game.BlackKing

    # Checking for enemy pawn attacks
    Checking_Piece = (WHITE_PAWN_ATKS[ King ] & Game.BlackPawn) if Colour else (BLACK_PAWN_ATKS[ King ] & Game.WhitePawn)
    if Checking_Piece: masks.append( Checking_Piece )

    # Checking for enemy knight attacks
    Checking_Piece = (KNIGHT_MOVES[ King ] & Game.BlackKnight) if Colour else (KNIGHT_MOVES[ King ] & Game.WhiteKnight)
    if Checking_Piece: masks.append( Checking_Piece )

    # Checking for enemy bishop attacks
    Checking_Piece = (Compute_Bishop_attacks( King,Game.AllPieces ) & Game.Ascii_To_Board('b' if Colour else 'B'))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,King ) ^ King )

    # Checking for enemy rook attacks
    Checking_Piece =  (Compute_Rook_attacks( King,Game.AllPieces ) & Game.Ascii_To_Board('r' if Colour else 'R'))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,King ) ^ King )

    # Checking for enemy queen attacks
    Checking_Piece = (Compute_Queen_attacks( King,Game.AllPieces ) & Game.Ascii_To_Board('q' if Colour else 'Q'))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,King ) ^ King )

    # Return False if there is no check
    if len(masks) == 0: return False

    # Return the singular masks if one piece is checking
    if len(masks) == 1: return masks[0]

    # Return the string "Double" to signify multiple piece are checking the king
    if len(masks) > 1: return 'Double'

# Returns a dictionary mapping the bitboards of pinned pieces to their relevant filters,
# filters are the attacking rays of the attacker, (pinned pieces may only move along this ray or capture said piece)
def Get_Pinned_Pieces(col:str, Game:GameState) -> dict:
    Pins = {}

    # Get location of relevant pieces
    FriendlyKing = Game.WhiteKing   if col == 'w' else Game.BlackKing
    FriendlyAll  = Game.WhiteAll    if col == 'w' else Game.BlackAll
    EnemyQueen   = Game.BlackQueen  if col == 'w' else Game.WhiteQueen
    EnemyRook    = Game.BlackRook   if col == 'w' else Game.WhiteRook
    EnemyBishop  = Game.BlackBishop if col == 'w' else Game.WhiteBishop


    # Get location of all enemy slider pieces aligned with our king
    Pinners = ( 

        # Enemy pieces on rook rays (horizontal / vertical)
        ( Compute_Rook_attacks( FriendlyKing, NoBits ) & ( EnemyQueen | EnemyRook ) ) |

        # Enemy pieces on bishop rays (diagonal / anti-diagonal)
        ( Compute_Bishop_attacks( FriendlyKing, NoBits ) & ( EnemyQueen | EnemyBishop ) ) 
              )
    
    # Iterate through all potential pinners and check if a pinned piece is on this ray
    while Pinners:

        # Gets potentially pinning piece
        Attacker = Get_LSB( Pinners )

        # Build the ray that joins the King and the potential pinner
        Ray = Build_Ray( FriendlyKing, Attacker ) ^ FriendlyKing # (Removes bit on the king)

        # Gets potentially pinned piece
        Pinned = FriendlyAll & Ray

        # Note that if there is more than 1 bit this isn't a pin 
        # because either piece may move without leaving the king in check
        if BitCount( Pinned ) == 1: 
            
            # Get the attacking ray from pinned piece to the attacker
            Filter = Build_Ray( Pinned, Attacker ) ^ Pinned # (Removes bit on the pinned piece)

            # Map this location to the calculated filter
            Pins[ Pinned ] = Filter

        # Remove this bit from the bitboard
        Pinners ^= Attacker
    
    return Pins

STARTING_FEN  = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
STARTING_GAME = GameState('w',STARTING_FEN,None,i8(0b1111),0,1)