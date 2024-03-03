#/*******************************************\
#                C L A S S E S   
#                - - - - - - - 
#\*******************************************/
from copy import deepcopy
import numpy as np
from Engine.ConstantsAndTables import *
from Engine.BitMacros          import *

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

    def __init__(self,side,source,target,capture,castle,piece,promo,EnPassant=False):
        self.Side      = side      # 'w' signifies white, 'b' signifies black
        self.Source    = source    # The index of the source square
        self.Target    = target    # The index of the target square
        self.Capture   = capture   # Is this move a capture ?
        self.Castle    = castle    # Is this move a castle ?
        self.Piece     = piece     # Standard chess notation applies (Capital = white, Lowercase = black)
        self.Promotion = promo     # Is this move a promotion ? (False or piece name)
        self.EnPassant = EnPassant # Is this move an en passant ?

# Class for storing all gamestate variables in one place
class GameState():
    
    # Initialises bitboards representing the game
    def InitBoards(self):
        # White pieces
        self.WhitePawn   = NoBits
        self.WhiteKnight = NoBits
        self.WhiteBishop = NoBits
        self.WhiteRook   = NoBits
        self.WhiteQueen  = NoBits
        self.WhiteKing   = NoBits
        # Black pieces
        self.BlackPawn   = NoBits
        self.BlackKnight = NoBits
        self.BlackBishop = NoBits
        self.BlackRook   = NoBits
        self.BlackQueen  = NoBits
        self.BlackKing   = NoBits
        # Names of attributes
        self.WhitePieceNames = ['WhitePawn', 'WhiteKnight', 'WhiteBishop', 'WhiteRook',
                                'WhiteQueen', 'WhiteKing']
        self.BlackPieceNames = ['BlackPawn', 'BlackKnight', 'BlackBishop', 'BlackRook',
                                'BlackQueen', 'BlackKing']
    
    # (The decorator @property allows this attribute to automatically update when one of the attributes in the list changes)
    # Piece combinations 
    @property
    def WhiteAll(self): 
        return (self.WhitePawn | self.WhiteKnight | self.WhiteBishop | self.WhiteRook | self.WhiteQueen | self.WhiteKing)
    @property
    def BlackAll(self):
        return (self.BlackPawn | self.BlackKnight | self.BlackBishop | self.BlackRook | self.BlackQueen | self.BlackKing)
    @property
    def AllPieces(self): return (self.WhiteAll | self.BlackAll)

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
        
        # Explanation of all attributes
        # ------------------------------
        # Side_To_Move:  Either 'w' or 'b' , signifying white or black respectively
        # FEN:  Fen string with normal FEN-notation 
        # En_Passant:  A target en-passant square stored as a string of the referenced square (e.g. 'e4')
        # Castle_Rights:  4-bit binary number (First 2 bits are white's queenside and kingside castle rights respectively)
        #                                     (Last 2 bits are black's queenside and kingside castle rights respectively) 
        # Half_Move_Clock:  The number of halfmoves since the last capture or pawn advance, used for the fifty-move rule.
        # Full_Move_Clock:  The number of the full moves. It starts at 1 and is incremented after Black's move
        # WhiteCheckMask:  A mask containing the attack rays of any checks on the white king
        # BlackCheckMask:  A mask containing the attack rays of any checks on the black king
        # Pins:  A dictionary mapping bitboard squares to their relevant attack rays if this piece is pinned
        #        (Pinned pieces can only move along the attack ray)
        # PreviousPositions:  List of previous board states
        # PreviousMoves:  List of previous moves

        self.Side_To_Move      = side
        self.FEN               = Pos
        self.En_Passant        = EnPassant
        self.Castle_Rights     = CastleRights
        self.Half_Move_Clock   = HalfMove
        self.Full_Move_Clock   = FullMove
        self.WhiteCheckMask    = AllBits
        self.BlackCheckMask    = AllBits 
        self.Pins              = {}
        self.PreviousPositions = []
        self.PreviousMoves     = []

        self.InitBoards()
        self.Parse_FEN( Pos )
        self.Game_Update()
    
    # Adds piece to bitboard
    def AddPiece(self,piece,square):

        # Get board then add piece to board then update that board attribute
        Board = Set_bit( Ascii_To_Board(self,piece),square )
        setattr( self, Ascii_To_Name[ piece ], Board )

    def Initialise_GameState_Variables(self,fen:str, Side:str, En_Passant:str|None, Half_Move:int, Full_Move:int, Castle:str):
        
        # Initialise gamestate attributes
        self.FEN           = fen
        self.Side_To_Move  = Side
        self.Castle_Rights = i8(0)
        self.En_Passant = None if (En_Passant == '-') else En_Passant
        self.Half_Move_Clock = Half_Move
        self.Full_Move_Clock = Full_Move
        if Castle == '-': return
        Get_Rights = {'K':W_KingSide, 'Q':W_QueenSide, 'k':B_KingSide, 'q':B_QueenSide}

        for letter in Castle: self.Castle_Rights |= Get_Rights[letter]

        self.PreviousPositions = [deepcopy(self)]
            
    # Takes a Forsyth-edwards notation string and prints the relevant game state
    def Parse_FEN(self,fen:str):
        # Separate fen into separate strings based on what information they have
        Fen, Side, Castle, En_Passant, Half_Move, Full_Move = fen.split()
        
        # Iterates through each rank and square of the board
        Square = 63
        for rank in Fen.split('/'):
            for piece in rank:

                # If there is a number in the fen, this indicates the number of empty squares found
                if piece.isdigit(): Square -= int( piece )

                # Otherwise place a piece on the relevant bitboard square
                else: 
                    self.AddPiece(piece, Square)
                    Square -= 1
       
        # Initialise gamestate varaibles
        self.Initialise_GameState_Variables(fen, Side, En_Passant, int(Half_Move), int(Full_Move), Castle)
       
    # Invalidates castling rights if rook or king moves
    def Update_Castle_Rights(self, move):
        Source = i64(2**move.Source)
        KingsideFlag  = W_KingSide  if (move.Side == 'w') else B_KingSide
        QueensideFlag = W_QueenSide if (move.Side == 'w') else B_QueenSide

        KingsideRights  = True if (self.Castle_Rights & KingsideFlag)  else False
        QueensideRights = True if (self.Castle_Rights & QueensideFlag) else False
        
        KingsideRookSQ  = SquareH1 if (move.Side == 'w') else SquareH8
        QueensideRookSQ = SquareA1 if (move.Side == 'w') else SquareA8
        
        # If this colour has no rights then we dont have to do anything
        if (self.Castle_Rights & (KingsideRights | QueensideRights)): return 

        # Remove these rights if they exist
        if move.Piece == 'K': 
            self.Castle_Rights ^= KingsideFlag  if (KingsideRights)  else NoBits
            self.Castle_Rights ^= QueensideFlag if (QueensideRights) else NoBits

        if move.Piece == 'R':
            self.Castle_Rights ^= KingsideFlag  if (KingsideRights)  and (Source == KingsideRookSQ)  else NoBits
            self.Castle_Rights ^= QueensideFlag if (QueensideRights) and (Source == QueensideRookSQ) else NoBits

    # Plays a given move onto the board
    def Make_Move(self,move:Move):

        # Get source and target bitboards
        Source = i64( 2**move.Source )
        Target = i64( 2**move.Target )

        # Updates to game clocks
        if move.Side == 'b': self.Full_Move_Clock += 1
        self.Half_Move_Clock += 1
        if move.Piece in ['P', 'p'] or move.Capture == True: self.Half_Move_Clock = 0

        # Invalidate castling rights if rook or king moves
        if move.Piece in ['K','k','R','r']: self.Update_Castle_Rights(move)
            
        if move.Castle == True: Make_Castle(self, Source, Target, move.Side)

        if move.Capture == True: Make_Capture(self,Target, move.Side)

        if move.Promotion == True: Make_Promotion(self, move)
        
        if move.Castle == False or move.Promotion == False:
            # Update actual bitboard                   Removes ↓  //  ↓ Adds 
            Board  = Ascii_To_Board(self, move.Piece) ^ ( Source | Target )
            setattr(self, Ascii_To_Name[move.Piece], Board)

        # Record previous move and board state
        self.PreviousPositions.append(deepcopy(self)) 
        self.PreviousMoves.append(move)

    # Unmakes the move just played
    def Unmake_Move(self):
        # If no previous positions then no moves to unmake
        if len(self.PreviousPositions) < 2: return

        # Set attributes of the current game state to the previous position
        self.PreviousPositions.pop()
        LastPosition = self.PreviousPositions[-1]

        for attr in vars(LastPosition):
            if attr == 'PreviousPositions': continue
            vars(self)[attr] = vars(LastPosition)[attr] 

    # Updates the current en passant square
    def Get_En_Passant(self):
        # Check for any en passant squares   //   Was last move by a pawn ?
        if (self.PreviousMoves == []) or self.PreviousMoves[-1].Piece not in ['P','p']: return

        LastMove = self.PreviousMoves[-1]

        # Was this a double move ? 
        if abs(LastMove.Target - LastMove.Source) == 16:
            if (LastMove.Side == 'w'): self.En_Passant = Index_to_square[LastMove.Target - 8]
            else:                      self.En_Passant = Index_to_square[LastMove.Target + 8]

        else: self.En_Passant = None

    # Updates all the GameState varibles
    def Game_Update(self):
        
        # Update dictionary containing all pins
        self.Pins = Get_Pinned_Pieces('w', self) | Get_Pinned_Pieces('b', self)

        # If there is check update the check mask
        self.WhiteCheckMask = Is_Check('w', self)
        self.BlackCheckMask = Is_Check('b', self)
        
        # Only get new en passant squares upon the event that a move was played
        if self.PreviousMoves != []: self.En_Passant = self.Get_En_Passant()
        
# Determines whether there is a check or not if so returns a bitboard of the attacking ray
# (Used in legal move generation)
@cache
def Is_Check(col:str, Game:GameState):
    # Will store rays or locations of checking pieces
    masks = []

    # Select correct king bitboard
    KingBoard  = Game.WhiteKing if (col=='w') else Game.BlackKing
    KingSquare = GetIndex(KingBoard) 

    # Get enemy pieces
    EnemyBishop = 'b' if (col=='w') else 'B'
    EnemyRook   = 'r' if (col=='w') else 'R'
    EnemyQueen  = 'q' if (col=='w') else 'Q'
    EnemyPawn   = ( (WHITE_PAWN_ATKS[ KingSquare ] & Game.BlackPawn) if (col=='w') else 
                    (BLACK_PAWN_ATKS[ KingSquare ] & Game.WhitePawn) )
    EnemyKnight = ( (KNIGHT_MOVES[ KingSquare ] & Game.BlackKnight) if (col=='w') else 
                    (KNIGHT_MOVES[ KingSquare ] & Game.WhiteKnight) )

    # Checking for enemy pawn and knight attacks
    if EnemyPawn:   masks.append( EnemyPawn )
    if EnemyKnight: masks.append( EnemyKnight )
    
    # The occupancy bitboard must not have the bit where the king is set to one otherwise the
    # algorithm for slider piece move generation doesn't return any moves
    Occupancy = Game.AllPieces ^ KingBoard

    Checking_Piece = (Compute_Bishop_attacks(KingBoard, Occupancy) & Ascii_To_Board(Game,EnemyBishop))
    if Checking_Piece: masks.append( Build_Ray(Checking_Piece,KingBoard) ^ KingBoard )

    Checking_Piece =  (Compute_Rook_attacks(KingBoard, Occupancy) & Ascii_To_Board(Game,EnemyRook))
    if Checking_Piece: masks.append( Build_Ray(Checking_Piece,KingBoard) ^ KingBoard )

    Checking_Piece = (Compute_Queen_attacks(KingBoard, Occupancy) & Ascii_To_Board(Game,EnemyQueen))
    if Checking_Piece: masks.append( Build_Ray(Checking_Piece,KingBoard) ^ KingBoard )

    # Return Allbits if there is no check, to allow all moves
    if len(masks) == 0: return AllBits
    
    # Return the mask if only one piece is checking otherwise return string: "Double"
    return masks[0] if (len(masks)==1) else 'Double'
    
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

        Attacker = Get_LSB( Pinners )

        # Finds potentially pinned pieces by looking for pieces on the attacking ray
        Pinned  = Delete_bit( Build_Ray(FriendlyKing, Attacker), GetIndex(FriendlyKing) )
        Pinned  = Delete_bit( Pinned, GetIndex(Attacker) ) 
        Pinned &= Game.AllPieces

        # Note that if there is more than 1 bit this isn't a pin
        # because either piece may move without leaving the king in check
        if BitCount(Pinned) == 1 and (FriendlyAll & Pinned): # Only our colour pieces may be pinned
            
            # Get the attacking ray from king to the attacker
            Filter = Build_Ray(FriendlyKing, Attacker) ^ FriendlyKing # (Removes bit on the king)
            Pins[ Pinned ] = Filter

        # Remove this bit from the bitboard
        Pinners ^= Attacker
    
    return Pins
           
# Plays a castling move onto the game board
def Make_Castle(Game:GameState, Source:i64, Target:i64, Colour:str):
    
    # Get correct king and rook bitboards
    King = Game.WhiteKing if Colour == 'w' else Game.BlackKing
    Rook = Game.WhiteRook if Colour == 'w' else Game.BlackRook

    # True if the king moves to the right
    Right = True if (Target < Source) else False

    # Move king and rook to correct square
    King ^= ( Source | Target )

    if Colour=='w':  Rook ^= (SquareH1|SquareF1) if (Right) else (SquareA1|SquareD1)
    #                         Current -> Target        ///        Current -> Target

    else:            Rook ^= (SquareH8|SquareF8) if (Right) else (SquareA8|SquareD8)
    #                         Current -> Target        ///        Current -> Target

    # Set correct attributes
    if Colour == 'w':
        setattr(Game, 'WhiteKing', King)
        setattr(Game, 'WhiteRook', Rook)
    else: 
        setattr(Game, 'BlackKing', King)
        setattr(Game, 'BlackRook', Rook)
    
# Plays a capture onto the game board
def Make_Capture(Game:GameState, Target:i64, Colour:str) -> None:

    # Gets the list of the names of enemy bitboards
    EnemyBoardNames = Game.BlackPieceNames if (Colour == 'w') else Game.WhitePieceNames

    # Iterate through enemy bitboards until we find the correct one
    for PieceName in EnemyBoardNames:
        EnemyBoard = vars(Game)[PieceName]
        
        # If this enemy bitboard contains the piece we captured
        if Remove := (EnemyBoard & Target): 
            
            EnemyBoard = Delete_bit(EnemyBoard, GetIndex(Remove))
            setattr(Game, PieceName, EnemyBoard)
            break

# Plays a promotion onto the game board
def Make_Promotion(Game:GameState, move:Move) -> None:
    Source = i64(2**move.Source)
    Target = i64(2**move.Target)

    # Remove pawn from board
    if move.Side == 'w': Game.WhitePawn ^= Source
    else:                Game.BlackPawn ^= Source
    
    Piece = move.Promotion.upper() if (move.Side == 'w') else move.Promotion.lower()
    
    # Add newly promoted piece to board
    vars(Game)[Ascii_To_Name[Piece]] ^= Target 

# Takes in an ascii representation and returns relevant bitboard
def Ascii_To_Board(Game:GameState, piece:str) -> i64:
    # Map each letter to relevant board
    Char_To_Board = {
        'P':Game.WhitePawn, 'p':Game.BlackPawn, 'N':Game.WhiteKnight, 'n':Game.BlackKnight,
        'B':Game.WhiteBishop, 'b':Game.BlackBishop, 'R':Game.WhiteRook, 'r':Game.BlackRook,
        'Q':Game.WhiteQueen, 'q':Game.BlackQueen, 'K':Game.WhiteKing, 'k':Game.BlackKing }

    return Char_To_Board[ piece ]
