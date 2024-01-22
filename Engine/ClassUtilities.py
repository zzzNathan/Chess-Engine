#/*******************************************\
#                C L A S S E S   
#                - - - - - - - 
#\*******************************************/

import numpy as np
from ConstantsAndTables import *

# Shorthands
i64 = np.uint64
i8  = np.uint8

# Sets a bit on bitboard, 0 to 1
def Set_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard | i64(2**SquareNum)

# Helper dictionary that maps piece ascii codes to their human readable names
Ascii_To_Name = {'P':'WhitePawn', 'p':'BlackPawn', 'N':'WhiteKnight', 'n':'BlackKnight',
                 'B':'WhiteBishop', 'b':'BlackBishop', 'R':'WhiteRook', 'r':'BlackRook',
                 'Q':'WhiteQueen', 'q':'BlackQueen', 'K':'WhiteKing', 'k':'BlackKing'}

# Class to give structure and order to moves and other relevant information
class Move():

    def __init__(self,side,source,target,capture,castle,piece,promo ) -> None:
        self.Side      = side    # 'w' signifies white, 'b' signifies black
        self.Source    = source  # The index of the source square
        self.Target    = target  # The index of the target square
        self.Capture   = capture # Is this move a capture ?
        self.Castle    = castle  # Is this move a castle ?
        self.Piece     = piece   # Standard chess notation applies (Capital = white, Lowercase = black)
        self.Promotion = promo   # Is this move a promotion ?

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

        # The number of the full moves. It starts at 1 and is incremented after Black's move
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

    # Updates current occupancy bitboards
    def UpdateOcc(self):

        # Ensures all occupancy boards are correct
        self.WhiteAll  = (self.WhitePawn | self.WhiteKnight | self.WhiteBishop | 
                          self.WhiteRook | self.WhiteQueen  | self.WhiteKing)
        
        self.BlackAll  = (self.BlackPawn | self.BlackKnight | self.BlackBishop |
                           self.BlackRook | self.BlackQueen | self.BlackKing)
        
        self.AllPieces = ( self.WhiteAll | self.BlackAll )
    
    # Adds piece to bitboard
    def AddPiece(self,piece,square):

        # Get name of board's variable
        Name = Ascii_To_Name[ piece ]

        # Get board to add piece to 
        Board  = Ascii_To_Board( self,piece )

        # Add piece to board
        Board = Set_bit( Board,square )

        # Update board attribute
        setattr( self,Name,Board )

    # Takes a Forsyth-edwards notation string and prints the relevant game state
    def Parse_FEN(self,fen:str):
        # Separate fen into separate strings based on what information they have
        Fen, Side, Castle, En_Passant, Half_Move, Full_Move = fen.split()
        
        # Iterates through each rank of the board
        Square = 63
        for rank in Fen.split('/'):
            
            # Iterate through pieces on each rank
            for piece in rank:

                # If there is a number in the fen, this indicates the number of empty squares found
                if piece.isdigit(): Square -= int( piece )

                # Otherwise place a piece on the relevant bitboard square
                else: 
                    self.AddPiece( piece,Square )
                    Square -= 1

        # Update gamestate attributes
        self.Side_To_Move = Side
        self.Castle_Rights = i8(0)
        self.En_Passant = None if (En_Passant == '-') else En_Passant
        self.Half_Move_Clock = Half_Move
        self.Full_Move_Clock = Full_Move
        if Castle == '-': return
        for letter in Castle:
            match letter:
                 
                case 'K': self.Castle_Rights != W_KingSide
                case 'Q': self.Castle_Rights |= W_QueenSide
                case 'k': self.Castle_Rights |= B_KingSide
                case 'q': self.Castle_Rights |= B_QueenSide

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
        Board  = Ascii_To_Board( self,Piece )
        Board ^= ( Source | Target )

        # Update the attribute to the new bitboard
        Name = Ascii_To_Name[ Piece ]
        setattr( self,Name,Board )
            
        # Update occupancies
        self.UpdateOcc()

# Plays a castling move onto the game board
def Make_Castle(Game:GameState, Source:i64, Target:i64, Colour:str):
    # Get correct king bitboard
    King = Game.WhiteKing if Colour == 'w' else Game.BlackKing

    # Get correct rook bitboard
    Rook = Game.WhiteRook if Colour == 'w' else Game.BlackRook

    # True if the king moves to the right
    Right = True if (Target < Source) else False

    # Move king
    King ^= ( Source | Target )

    # Move rook
    if Right:
        if Colour == 'w': Rook ^= ( SquareH1 | SquareF1 )
        else:             Rook ^= ( SquareH8 | SquareF8 )
    else:
        if Colour == 'w': Rook ^= ( SquareA1 | SquareD1 )
        else:             Rook ^= ( SquareA8 | SquareD8 )

# Plays a capture onto the game board
def Make_Capture(Game:GameState, Target:i64, Colour:str):

    # Gets the list of enemy bitboards
    EnemyBoards = Game.BlackPieces if Colour == 'w' else Game.WhitePieces

    # Iterate through enemy bitboards until we find the correct one
    for EnemyPiece in EnemyBoards:

        # If this enemy bitboard contains the piece we captured
        if Remove := (EnemyPiece & Target): 

            # Remove this piece from the board
            EnemyPiece ^= Remove
            break

# Takes in an ascii representation and returns relevant bitboard
def Ascii_To_Board(Game:GameState, piece:str) -> i64:
    # Map each letter to relevant board
    Char_To_Board = {
        'P':Game.WhitePawn, 'p':Game.BlackPawn, 'N':Game.WhiteKnight, 'n':Game.BlackKnight,
        'B':Game.WhiteBishop, 'b':Game.BlackBishop, 'R':Game.WhiteRook, 'r':Game.BlackRook,
        'Q':Game.WhiteQueen, 'q':Game.BlackQueen, 'K':Game.WhiteKing, 'k':Game.BlackKing }

    # Return the board that was required
    return Char_To_Board[ piece ]
