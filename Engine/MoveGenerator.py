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
from ConstantsAndTables import *
from Utilities import *

# Shorthands
i64 = np.uint64
i8 = np.uint8

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
        else:
            
            # Add to movelist
            MoveList.append( Move(col,SourceIndex,TargetIndex,False,False,piece,False) )

        # Remove this bit from the board
        bitboard ^= CurrentBB

    return MoveList

# Will generate correct move filter in case piece is pinned or king is in check
def Generate_Filter(Game:GameState, move:Move) -> i64:

    # Get source and target square bitboards
    SourceBB = i64( 2**move.Source )
    TargetBB = i64( 2**move.Target )

    # Verifies whether the piece's king is currently in check, if so gives the 
    # relevant check mask
    Is_King_in_Check = Game.WhiteCheckMask if move.Side == 'w' else Game.BlackCheckMask

    # Checks if there is a check on the piece's king
    if Is_King_in_Check: return Is_King_in_Check
    
    # Checks if this piece is pinned
    if SourceBB in Game.Pins: return Game.Pins[ SourceBB ]

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

    # Iterate through all given moves
    for move in movelist:

        # If this move passed the filter add it to the list
        if Filter_Move( Game,move ):FilteredMoveList.append(move)

    return FilteredMoveList

# Takes in a piece's ascii representation and return a funtion that generates their pseudo-legal moves
def Find_Move_Table(Game:GameState, char:str):
    char = char.upper()

    # Knights
    if char == 'N': 
        return lambda bitboard : KNIGHT_MOVES[ Board_To_Square(bitboard) ]

    # Bishops
    if char == 'B': 
        return lambda bitboard : Compute_Bishop_attacks(bitboard,Game.AllPieces) 
    
    # Rooks
    if char == 'R':
        return lambda bitboard : Compute_Rook_attacks(bitboard,Game.AllPieces)
    
    # Queens
    if char == 'Q':
        return lambda bitboard : Compute_Queen_attacks(bitboard,Game.AllPieces)

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

https://github.com/Avo-k/black_numba/blob/master/attack_tables.py
'''

def GeneratePromotions(Source:i64, Target:i64, col:str, Capture:bool) -> list:
    # Getting index of promotion squares
    SourceIndex = int( math.log2(Source) )
    TargetIndex = int( math.log2(Target) )

    # Get pawn's piece representation
    piece = 'P' if col == 'w' else 'p'

    MoveList = []
    # Adding knight promotion    //    Evaluates to 1 if 'col'='w' (white) otherwise 0 and indexes at this number   
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'nN'[col=='w']) )
    
    # Adding bishop promotion    //    Evaluates to 1 if 'col'='w' (white) otherwise 0 and indexes at this number  
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'bB'[col=='w']) )

    # Adding rook promotion      //    Evaluates to 1 if 'col'='w' (white) otherwise 0 and indexes at this number  
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'rR'[col=='w']) )

    # Adding queen promotion     //    Evaluates to 1 if 'col'='w' (white) otherwise 0 and indexes at this number  
    MoveList.append( Move(col,SourceIndex,TargetIndex,Capture,False,piece,'qQ'[col=='w']) )

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

        # Obstruction check for one square up
        if not Is_Obstructed( Game,SourceUpOne ):Target = WHITE_PAWN_MOVES[Index] 

        # Obstruction check for two square up                      Remove this bit if obstructed
        if Is_Second_Rank(Source) and Is_Obstructed( Game,SourceUpTwo ): Target ^= SourceUpTwo

        # Add these moves to the list
        MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','P') )

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

        # Add all non-capture promotions 
        if Is_Seventh_Rank(Source) and (not Is_Obstructed(Game,SourceUpOne)):

            # Add moves to list
            MoveList.extend( GeneratePromotions(Source,SourceUpOne,'w',False) )

        # Add en passant moves
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if BLACK_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:

                # Add to movelist
                MoveList.append( Move('w',Index,Square_to_index[Game.En_Passant],True,False,'P') )    

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

        # Obstruction check for one square up
        if not Is_Obstructed(Game,SourceUpOne): Target = BLACK_PAWN_MOVES[Index]

        # Obstruction check for two square up                      Remove this bit if obstructed
        if Is_Seventh_Rank(Source) and Is_Obstructed(Game,SourceUpTwo): Target ^= SourceUpTwo

        # Add these moves to the list
        MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','p') )

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

        # Add all non-capture promotions 
        if Is_Second_Rank(Source) and (not Is_Obstructed(Game,SourceUpOne)):

            # Add moves to list
            MoveList.extend( GeneratePromotions(Source,SourceUpOne,'b',False) )

        # Add en passant moves
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if WHITE_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:

                # Add to movelist
                MoveList.append( Move('b',Index,Square_to_index[Game.En_Passant],True,False,'p') )    
    
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

    # Get white king bitboard, (there is only one king)
    Source = board_copy

    # Get source square index for move encoding
    Index = Board_To_Square(Source)

    # Generate pseudo-legal moves
    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------

    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & All_Attacked_squares('b')) | (Target & Game.WhiteAll)

    # Add in all regular moves
    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','K') )

    # Castling Moves
    # ---------------------------------------------------------------------------

    # Helper bitboards that verify castling moves 
    # These varibles only need to be initialised if the king is on the castling square
    if Source == SquareE1:
        KingRightCastle = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle  = (Source << i64(1)) | (Source << i64(2)) 
        KingRightOne    = (Source >> i64(1)) 
        KingLeftOne     = (Source << i64(1))
        KingRightTwo    = (Source >> i64(2))
        KingLeftTwo     = (Source << i64(2))

    # Generate castling moves
    # Kingside:                     Verify king is on correct square            Verify obstructions
    if Game.Castle_Rights & i8(0b0100) and Source == SquareE1 and ( not Is_Obstructed(Game,KingRightCastle) ): 

        # Verify we aren't moving through check
        if not ( Is_square_attacked(KingRightOne,'b') or Is_square_attacked(KingRightTwo,'b') ):

            # Add to move list
            MoveList.append( Move('w',Index,Index - 2,False,True,'K',False) )

    # Queenside:                     Verify king is on correct square           Verify obstructions
    if Game.Castle_Rights & i8(0b1000) and Source == SquareE1 and ( not Is_Obstructed(Game,KingLeftCastle) ): 

        # Verify we aren't moving through check
        if not ( Is_square_attacked(KingLeftOne,'b') or Is_square_attacked(KingLeftTwo,'b') ):

            # Add to move list
            MoveList.append( Move('w',Index,Index + 2,False,True,'K',False) )

    return MoveList

def Generate_Black_King_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    # Get black king bitboard, (there is only one king)
    Source = board_copy

    # Get source square index for move encoding
    Index = Board_To_Square(Source)

    # Generate pseudo-legal moves
    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------

    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & All_Attacked_squares('w')) | (Target & Game.BlackAll)

    # Add in all regular moves
    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','k') )

    # Castling Moves
    # ---------------------------------------------------------------------------

    # Helper bitboards that verify castling moves 
    # These varibles only need to be initialised if the king is on the castling square
    if Source == SquareE8:
        KingRightCastle = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle  = (Source << i64(1)) | (Source << i64(2)) 
        KingRightOne    = (Source >> i64(1)) 
        KingLeftOne     = (Source << i64(1))
        KingRightTwo    = (Source >> i64(2))
        KingLeftTwo     = (Source << i64(2))

    # Generate castling moves
    # Kingside:                   Verify king is on correct square              Verify obstructions
    if Game.Castle_Rights & i8(0b0001) and Source == SquareE8 and ( not Is_Obstructed(Game,KingRightCastle) ): 

        # Verify we aren't moving through check
        if not ( Is_square_attacked(KingRightOne,'w') or Is_square_attacked(KingRightTwo,'w') ):

            # Add to move list
            MoveList.append( Move('b',Index,Index - 2,False,True,'k',False) )

    # Queenside:                   Verify king is on correct square             Verify obstructions
    if Game.Castle_Rights & i8(0b0010) and Source == SquareE8 and ( not Is_Obstructed(Game,KingLeftCastle) ): 

        # Verify we aren't moving through check
        if not ( Is_square_attacked(KingLeftOne,'w') or Is_square_attacked(KingLeftTwo,'w') ):

            # Add to move list
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

    # Generate moves for the correct colour
    BOARDS = Game.WhitePieces if col == 'w' else Game.BlackPieces

    # Iterate through all bitboards and generate moves for all candidate pieces
    for bitboard in BOARDS:
        # Create a copy of the bitboard
        board_copy = bitboard

        # Generate white pawn moves
        if bitboard == Game.WhitePawn:
            MoveList.extend( Generate_White_Pawn_Moves(board_copy,Game) )

        # Generate black pawn moves
        elif bitboard == Game.BlackPawn:
            MoveList.extend( Generate_Black_Pawn_Moves(board_copy,Game) )

        # Generate white knight moves
        elif bitboard == Game.WhiteKnight:
            MoveList.extend( Generate_White_Knight_Moves(board_copy,Game) )
        
        # Generate black knight moves
        elif bitboard == Game.BlackKnight:
            MoveList.extend( Generate_Black_Knight_Moves(board_copy,Game) )

        # Generate white bishop moves
        elif bitboard == Game.WhiteBishop:
            MoveList.extend( Generate_White_Bishop_Moves(board_copy,Game) )

        # Generate black bishop moves
        elif bitboard == Game.BlackBishop:
            MoveList.extend( Generate_Black_Bishop_Moves(board_copy,Game) )

        # Generate white rook moves
        elif bitboard == Game.WhiteRook:
            MoveList.extend( Generate_White_Rook_Moves(board_copy,Game) )

        # Generate black rook moves 
        elif bitboard == Game.BlackRook:
            MoveList.extend( Generate_Black_Rook_Moves(board_copy,Game) )

        # Generate white queen moves
        elif bitboard == Game.WhiteQueen:
            MoveList.extend( Generate_White_Queen_Moves(board_copy,Game) )
        
        # Generate black queen moves
        elif bitboard == Game.BlackQueen:
            MoveList.extend( Generate_Black_Queen_Moves(board_copy,Game) )

        # Generate white king moves
        elif bitboard == Game.WhiteKing:
            MoveList.extend( Generate_White_King_Moves(board_copy,Game) )
        
        # Generate black king moves
        elif bitboard == Game.BlackKing:
            MoveList.extend( Generate_Black_King_Moves(board_copy,Game) )

    return Filter_All_Moves( Game,MoveList )

# TESTING CODE
'''
for move in Generate_Moves(STARTING_GAME,'w'):
   #print( move[10:16],2)
   #print( move[0],move[1:4],Index_to_square[int(f'0b{move[4:10]}',2)],Index_to_square[int(f'0b{move[10:16]}',2)],move[16:] )
   print(move)'''
