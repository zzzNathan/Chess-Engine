#/*******************************************\
#        M O V E   G E N E R A T I O N       
#        - - - -   - - - - - - - - - -
#\*******************************************/

'''
"We should forget about small efficiencies, say about 97% of the time: 
 PREMATURE OPTIMISATION IS THE ROOT OF ALL EVIL."
''' 
# Chess engine in Python
import numpy as np
from typing import Callable
from Engine.ConstantsAndTables import *
from Engine.Utilities          import *

# Shorthands
i64 = np.uint64
i8  = np.uint8

# Generates a list of moves that have target squares as the 1's on a given bitboard
def Create_Moves_From_Board(Game:GameState,source:i64,bitboard:i64,col:str,piece:str) -> list:
    MoveList    = []
    SourceIndex = GetIndex( source )
    # Get enemy piece bitboard in order to determine whether a move is a capture
    EnemyPieces = Game.WhiteAll if col == 'b' else Game.BlackAll

    # Loop over all precomputed moves and add them to the move list
    while bitboard:

        # Get source bitboard and it's index
        CurrentBB, TargetIndex = Get_LSB_and_Index( bitboard )

        # Capture moves
        if CurrentBB & EnemyPieces:

            # Add to movelist
            MoveList.append( Move(col,SourceIndex,TargetIndex,True,False,piece,False) )

        # Non-Capture moves
        else: MoveList.append( Move(col,SourceIndex,TargetIndex,False,False,piece,False) )

        # Remove this bit from the board
        bitboard ^= CurrentBB

    return MoveList

# Will generate correct move filter in case piece is pinned or king is in check
def Generate_Filter(Game:GameState, move:Move) -> i64:
    
    # If this is a king move we dont need to generate a filter
    # It's instead handled inside of the king move generator function
    if move.Piece in ['K','k']: return i64( 2**move.Target )
    
    # Get source and target square bitboards
    SourceBB = i64( 2**move.Source )
    TargetBB = i64( 2**move.Target )

    # Generates relevant checkmask
    CheckMask = Game.WhiteCheckMask if (move.Side == 'w') else Game.BlackCheckMask
    
    # If there is a check and piece is pinned piece must move to a square where both
    # filters are satisfied
    if SourceBB in Game.Pins: 
        if Is_Check(move.Side, Game) == AllBits: return Game.Pins[ SourceBB ] 
        else: return (Game.Pins[SourceBB] & CheckMask) 

    if Is_Check(move.Side, Game) != AllBits: 
        if move.EnPassant == False: return CheckMask
        
        # Ensure that the pawn being captured is in the check mask
        else: 
            CapturedPassant = (i64(2**(Square_to_index[Game.En_Passant] - 8))
                               if (move.Side == 'w') else
                               i64(2**(Square_to_index[Game.En_Passant] + 8)) )
            
            return (TargetBB
                    if (CapturedPassant & CheckMask)
                    else NoBits)

    # If this piece is neither pinned or has a king in check then return the move
    return TargetBB

# Will take in a given move and check if its legal
def Filter_Move(Game:GameState, move:Move) -> Move | bool:

    # Get target square bitboards
    TargetBB = i64( 2**move.Target )

    # Generate relevant filter
    Filter = Generate_Filter( Game,move )

    # If the move has passed through the filter return it, otherwise return false
    if TargetBB & Filter: return move
    else: return False

# Goes through all moves and ensures that they are all legal
def Filter_All_Moves(Game:GameState, movelist:list[Move]) -> list[Move]:
    FilteredMoveList = []

    # Iterate through all given moves and add them to list if they pass the filter
    for move in movelist:

        if Filter_Move( Game,move ):FilteredMoveList.append(move)

    return FilteredMoveList

# Takes in a piece's ascii representation and return a funtion that generates their pseudo-legal moves
def Find_Move_Table(Game:GameState, char:str) -> Callable:
    char = char.upper()

    # Knights
    if char == 'N': 
        return lambda bitboard : KNIGHT_MOVES[ Board_To_Square(bitboard) ]

    # Bishops
    if char == 'B': 
        return lambda bitboard : Compute_Bishop_attacks(bitboard, Game.AllPieces^bitboard) 
    
    # Rooks
    if char == 'R':
        return lambda bitboard : Compute_Rook_attacks(bitboard,Game.AllPieces^bitboard)
    
    # Queens
    if char == 'Q':
        return lambda bitboard : Compute_Queen_attacks(bitboard,Game.AllPieces^bitboard)

# Take in a given piece bitboard and generate all pseudo-legal moves
# (For all pieces except pawns and king)
def Gen_Pseudo_legal_Moves(Game:GameState, board:i64, char:str) -> list[Move]:
    MoveList = []

    # Is piece white?
    White       = char.isupper() 
    colour      = 'w' if (White) else 'b'

    # Gets a function that returns relevant moves for specified piece
    MoveFinder  = Find_Move_Table(Game, char)

    # Gets location of all friendly pieces
    FriendlyAll = Game.WhiteAll if (White) else Game.BlackAll

    # Iterate through all pieces on the board
    while board:

        # Get lowest bit on the board
        Current = Get_LSB(board)

        # Get all target moves
        Target = MoveFinder(Current)

        # Filter out all moves that capture friendly pieces
        Target ^=  (Target & FriendlyAll)

        # Add moves to list
        MoveList.extend( Create_Moves_From_Board(Game,Current,Target,colour,char) )

        # Remove this bit from the board
        board ^= Current

    return MoveList

'''
- For pins keep a dict in the game class 
- Ensure to check for pins and checks every game loop
- If a double check occurs then we will set the mask to 'Double' 
- Keep all previous positions in a table to check for 3-fold repitition

https://github.com/Avo-k/black_numba/blob/master/attack_tables.py
'''

def GeneratePromotions(Source:i64, Target:i64, col:str, Capture:bool) -> list:
    # Getting index of promotion squares
    SourceIndex = int( math.log2(Source) )
    TargetIndex = int( math.log2(Target) )

    # Get pawn's piece representation
    piece = 'P' if col == 'w' else 'p'

    MoveList = []
    # Adding knight promotion        
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'n') )
    
    # Adding bishop promotion    
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'b') )

    # Adding rook promotion      
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'r') )

    # Adding queen promotion     
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'q') )

    return MoveList

def Generate_White_Pawn_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    # Loop over all white pawns in the WhitePawns bitboard
    while board_copy:

        # Get source bitboard and it's index
        Source, Index = Get_LSB_and_Index(board_copy) 

        # Bitboards to indicate the piece being one and two squares up
        SourceUpOne = Source << i64(8)
        SourceUpTwo = Source << i64(16)

        # Normal Moves
        # -----------------------------------------------------------------------
        Target = i64(0) # Initialise target variable

        # Obstruction check for one square up / Cannot move without promotion on 7th rank
        if not Is_Obstructed(Game, SourceUpOne) and not Is_Seventh_Rank(Source):

            Target = WHITE_PAWN_MOVES[Index] 

            # Obstruction check for two square up              Remove this bit if obstructed
            if Is_Second_Rank(Source) and Is_Obstructed( Game,SourceUpTwo ): Target ^= SourceUpTwo

        MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','P') )

        # Add all non-capture promotions 
        if Is_Seventh_Rank(Source) and (not Is_Obstructed(Game,SourceUpOne)):

            MoveList.extend( GeneratePromotions(Source,SourceUpOne,'w',False) )

        # Attacking Moves
        # -----------------------------------------------------------------------

        # Add all attacking moves
        Target = WHITE_PAWN_ATKS[Index] & Game.BlackAll

        # Handle capture promotions
        if Is_Seventh_Rank(Source) and Is_Eigth_Rank(Target):
            
            # Loop through all bits and add these individual moves
            while Target:

                # Get target square bitboard
                CurrentBB = Get_LSB(Target)

                # Add these capture promotions to the movelist
                MoveList.append( GeneratePromotions(Source,CurrentBB,'w',True) )

                # Remove this bit from the bitboard
                Target ^= CurrentBB
        
        # Handle normal captures and add them to the list
        else: MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','P') )

        # Special Moves
        # -----------------------------------------------------------------------
        # Add en passant moves
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if BLACK_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:

                MoveList.append( Move('w',Index,Square_to_index[Game.En_Passant],True,False,'P',False,True) )

        # Remove this bit from the board
        board_copy ^= Source

    return MoveList

def Generate_Black_Pawn_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    # Loop over black pawns in the BlackPawns bitboard
    while board_copy:
        
        # Get source bitboard and it's index
        Source, Index = Get_LSB_and_Index(board_copy)

        # Bitboards to indicate the piece being one and two squares up
        SourceUpOne = Source >> i64(8)
        SourceUpTwo = Source >> i64(16)

        # Normal Moves
        # -----------------------------------------------------------------------
        Target = i64(0) # Initialise target variable

        # Obstruction check for one square up / Cannot move without promotion on 2nd rank
        if not Is_Obstructed(Game,SourceUpOne) and (not Is_Second_Rank(Source)): 

            Target = BLACK_PAWN_MOVES[Index]

            # Obstruction check for two square up             Remove this bit if obstructed ↓
            if Is_Seventh_Rank(Source) and Is_Obstructed(Game,SourceUpTwo): Target ^= SourceUpTwo

        MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','p') )

        # Add all non-capture promotions 
        if Is_Second_Rank(Source) and (not Is_Obstructed(Game,SourceUpOne)):
        
            MoveList.extend( GeneratePromotions(Source,SourceUpOne,'b',False) )

        # Attacking Moves
        # -----------------------------------------------------------------------

        # Add all attacking moves
        Target = BLACK_PAWN_ATKS[Index] & Game.WhiteAll

        # Handle capture promotions
        if Is_Second_Rank(Source) and Is_First_Rank(Target):
            
            # Loop through all bits and add these individual moves
            while Target:

                # Get target square bitboard
                CurrentBB = Get_LSB(Target)

                # Add these capture promotions to the movelist
                MoveList.append( GeneratePromotions(Source,CurrentBB,'b',True) )

                # Remove this bit from the bitboard
                Target ^= CurrentBB
        
        # Handle normal captures and add them to the list
        else: MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','p') )
        
        # Special Moves
        # -----------------------------------------------------------------------

        # Add en passant moves
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if WHITE_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:

                MoveList.append( Move('b',Index,Square_to_index[Game.En_Passant],True,False,'p',False,True) )    
    
        # Remove this bit from the board
        board_copy ^= Source

    return MoveList

def Generate_White_Knight_Moves(board_copy:i64, Game:GameState) -> list: 
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'N')
    

def Generate_Black_Knight_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'n')

def Generate_White_Bishop_Moves(board_copy:i64, Game:GameState) -> list:
     
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'B')

def Generate_Black_Bishop_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'b')

def Generate_White_Rook_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'R')

def Generate_Black_Rook_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'r')

def Generate_White_Queen_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'Q')

def Generate_Black_Queen_Moves(board_copy:i64, Game:GameState) -> list:
    
    # Return pseudo-legal moves
    return Gen_Pseudo_legal_Moves(Game,board_copy,'q')

def Generate_White_King_Moves(board_copy:i64, Game:GameState) -> list:

    MoveList = []
    Source = board_copy
    Index = Board_To_Square(Source)

    # Generate pseudo-legal moves
    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------
    # Get squares that are illegal to move to 
    IllegalSquares = All_Attacked_squares('b', Game, Source)

    # If the attacked squares includes the location of our king then the removing of illegal moves will fail
    IllegalSquares = Delete_bit(IllegalSquares, Index)

    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & IllegalSquares) | (Target & Game.WhiteAll)

    # Add in all regular moves
    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','K') )

    # Castling Moves
    # ---------------------------------------------------------------------------
    
    # Castles can only take place on E1
    if Source == SquareE1:
        KingRightCastle = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle  = (Source << i64(1)) | (Source << i64(2)) | (Source << i64(3))
        KingLeftAtkVerify = KingLeftCastle ^ (Source << i64(3))
        
        #     Validate kingside castle rights      //      Verify there are no obstructions
        if ( (Game.Castle_Rights & W_KingSide) and (not Is_Obstructed(Game,KingRightCastle)) and
        #             Ensure these squares arent attacked       //       Ensure there is no check   
             (not(All_Attacked_squares('b',Game) & KingRightCastle)) and (Game.WhiteCheckMask == AllBits) ):

            MoveList.append( Move('w',Index,Index - 2,False,True,'K',False) )

        
        #     Validate queenside castle rights      //      Verify there are no obstructions
        if ( (Game.Castle_Rights & W_QueenSide) and (not Is_Obstructed(Game,KingLeftCastle)) and
        #             Ensure these squares arent attacked       //       Ensure there is no check   
             (not(All_Attacked_squares('b',Game) & KingLeftAtkVerify)) and (Game.WhiteCheckMask == AllBits) ):

            MoveList.append( Move('w',Index,Index + 2,False,True,'K',False) )

    return MoveList

def Generate_Black_King_Moves(board_copy:i64, Game:GameState) -> list:
    
    MoveList = []
    Source = board_copy
    Index = Board_To_Square(Source)

    # Generate pseudo-legal moves
    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------
    # Get square that are illegal to move to 
    IllegalSquares = All_Attacked_squares('w', Game, Source)
    
    # If the attacked squares includes the location of our king then the removing of illegal moves will fail
    IllegalSquares = Delete_bit(IllegalSquares, Index)
    
    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & IllegalSquares) | (Target & Game.BlackAll)

    # Add in all regular moves
    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','k') )

    # Castling Moves
    # ---------------------------------------------------------------------------
    
    # Castles can only take place on E8
    if Source == SquareE8:
        KingRightCastle = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle  = (Source << i64(1)) | (Source << i64(2)) | (Source << i64(3))
        KingLeftAtkVerify = KingLeftCastle ^ (Source << i64(3))

        #     Validate kingside castle rights      //      Verify there are no obstructions
        if ( (Game.Castle_Rights & B_KingSide) and (not Is_Obstructed(Game,KingRightCastle)) and
        #             Ensure these squares arent attacked       //       Ensure there is no check   
             (not(All_Attacked_squares('w',Game) & KingRightCastle)) and (Game.BlackCheckMask == AllBits) ):

            MoveList.append( Move('b',Index,Index - 2,False,True,'k',False) )

        #     Validate queenside castle rights      //      Verify there are no obstructions
        if ( (Game.Castle_Rights & B_QueenSide) and (not Is_Obstructed(Game,KingLeftCastle)) and
        #             Ensure these squares arent attacked       //       Ensure there is no check   
             (not(All_Attacked_squares('w',Game) & KingLeftAtkVerify)) and (Game.BlackCheckMask == AllBits) ):

            MoveList.append( Move('b',Index,Index + 2,False,True,'k',False) )

    return MoveList

def Generate_Moves(Game:GameState, col:str) -> list:
    # List of all legal move in position
    MoveList = []

    # Is there currently a check ?
    Check = Is_Check(col,Game)
    
    # If there is a double check only the king may move
    if Check == 'Double':
        if col == 'w': return Generate_White_King_Moves( Game.WhiteKing,Game )
        else: return Generate_Black_King_Moves( Game.BlackKing,Game )

    # Generate all moves
    MoveList.extend(Generate_White_Pawn_Moves(Game.WhitePawn, Game)
                    if (col=='w') else
                    Generate_Black_Pawn_Moves(Game.BlackPawn, Game))

    MoveList.extend(Generate_White_Knight_Moves(Game.WhiteKnight, Game)
                    if (col=='w') else
                    Generate_Black_Knight_Moves(Game.BlackKnight, Game))

    MoveList.extend(Generate_White_Bishop_Moves(Game.WhiteBishop, Game)
                    if (col=='w') else
                    Generate_Black_Bishop_Moves(Game.BlackBishop, Game))

    MoveList.extend(Generate_White_Rook_Moves(Game.WhiteRook, Game)
                    if (col=='w') else
                    Generate_Black_Rook_Moves(Game.BlackRook, Game))

    MoveList.extend(Generate_White_Queen_Moves(Game.WhiteQueen, Game)
                    if (col=='w') else
                    Generate_Black_Queen_Moves(Game.BlackQueen, Game))

    MoveList.extend(Generate_White_King_Moves(Game.WhiteKing, Game)
                    if (col=='w') else
                    Generate_Black_King_Moves(Game.BlackKing, Game))

    return Filter_All_Moves( Game,MoveList )

# TESTING CODE

if __name__ == "__main__":
    bug = r'1r4k1/p6p/1n4pP/2b2pP1/2p1KP2/8/1P1R4/7R w - f6 0 36'
    ga = Fen_to_GameState(bug)
    print( Show_Board(ga) )
    print('\n\n')
    print([Move_To_UCI(m) for m in Filter_All_Moves(ga,Generate_White_Pawn_Moves(ga.WhitePawn, ga))])

'''
print( len(Generate_Moves(STARTING_GAME,'b') ))
for move in Generate_Moves(STARTING_GAME,'b'):
    print( Show_bitboard(i64(2**move.Source)) , '\n\n')
    print( Show_bitboard(i64(2**move.Target)) , '\n\n')
print('\n\n')
#print( len(Generate_Moves(STARTING_GAME,'b')) )
#for move in Generate_Moves(STARTING_GAME,'b'): print( move.__dict__ )'''
