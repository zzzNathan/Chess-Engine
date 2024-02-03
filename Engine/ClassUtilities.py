#/*******************************************\
#                C L A S S E S   
#                - - - - - - - 
#\*******************************************/

import numpy as np
from ConstantsAndTables import *
from BitMacros          import *

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
    
    # (The decorator @property allows this attribute to automatically update when one of the attributes in the list changes)

    # Piece combinations 
    @property
    def WhiteAll(self): 
        return (self.WhitePawn | self.WhiteKnight | self.WhiteBishop | self.WhiteRook | self.WhiteQueen | self.WhiteKing)

    @property
    def BlackAll(self):
        return (self.BlackPawn | self.BlackKnight | self.BlackBishop | self.BlackRook | self.BlackQueen | self.BlackKing)

    @property
    def AllPieces(self):
        return (self.WhiteAll | self.BlackAll)

    # Piece iterators 
    @property
    def WhitePieces(self):
        return [self.WhitePawn, self.WhiteKnight, self.WhiteBishop, self.WhiteRook, self.WhiteQueen, self.WhiteKing]

    @property
    def BlackPieces(self):
        return [self.BlackPawn, self.BlackKnight, self.BlackBishop, self.BlackRook, self.BlackQueen, self.BlackKing]

    @property
    def Pieces(self): return [*self.WhitePieces, *self.BlackPieces]

    # Initialising all attributes for the game
    def __init__(self,side:str,Pos:str,EnPassant:str|None,CastleRights:i8,HalfMove:int,FullMove:int):
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

    def Initialise_GameState_Variables(self, Side:str, En_Passant:str|None, Half_Move:int, Full_Move:int, Castle:str):
        
        # Initialise gamestate attributes
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
       
        Half_Move = int(Half_Move)
        Full_Move = int(Full_Move)
        # Initialise gamestate varaibles
        self.Initialise_GameState_Variables(Side, En_Passant, Half_Move, Full_Move, Castle)
        
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

    # Updates all the GameState varibles
    def Game_Update(self):
        
        # Update dictionary containing all pins
        self.Pins = Get_Pinned_Pieces('w', self) | Get_Pinned_Pieces('b', self)


 # Determines whether there is a check or not if so returns a bitboard of the attacking ray
# (Used in legal move generation)
def Is_Check(col:str, Game:GameState):
    # Will store rays or locations of checking pieces
    masks = []

    # Shorthand for colour checks 
    Colour = True if col == 'w' else False
    
    # Select correct king bitboard
    KingBoard  = Game.WhiteKing if Colour else Game.BlackKing
    KingSquare = GetIndex(KingBoard) 

    # Get enemy pieces
    EnemyBishop = 'b' if Colour else 'B'
    EnemyRook   = 'r' if Colour else 'R'
    EnemyQueen  = 'q' if Colour else 'Q'
    EnemyPawn   = ( (WHITE_PAWN_ATKS[ KingSquare ] & Game.BlackPawn) if Colour else 
                    (BLACK_PAWN_ATKS[ KingSquare ] & Game.WhitePawn) )
    EnemyKnight = ( (KNIGHT_MOVES[ KingSquare ] & Game.BlackKnight) if Colour else 
                    (KNIGHT_MOVES[ KingSquare ] & Game.WhiteKnight) )

    # Checking for enemy pawn attacks
    if EnemyPawn: masks.append( EnemyPawn )

    # Checking for enemy knight attacks
    if EnemyKnight: masks.append( EnemyKnight )

    # Checking for enemy bishop attacks
    Checking_Piece = (Compute_Bishop_attacks( KingBoard,Game.AllPieces ) & Ascii_To_Board(Game,EnemyBishop))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,KingBoard ) ^ KingBoard )

    # Checking for enemy rook attacks
    Checking_Piece =  (Compute_Rook_attacks( KingBoard,Game.AllPieces ) & Ascii_To_Board(Game,EnemyRook))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,KingBoard ) ^ KingBoard )

    # Checking for enemy queen attacks
    Checking_Piece = (Compute_Queen_attacks( KingBoard,Game.AllPieces ) & Ascii_To_Board(Game,EnemyQueen))
    if Checking_Piece: masks.append( Build_Ray( Checking_Piece,KingBoard ) ^ KingBoard )

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

STARTING_FEN  = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
STARTING_GAME = GameState('w',STARTING_FEN,None,i8(0b1111),0,1)
