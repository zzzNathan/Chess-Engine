#/*******************************************\
#        M O V E   G E N E R A T I O N       
#        - - - -   - - - - - - - - - -
#\*******************************************/
from Engine.MoveGen.MoveGenUtils import *

# References:
# - https://github.com/Avo-k/black_numba/blob/master/attack_tables.py

# To do:
# - Keep all previous positions in a table to check for 3-fold repitition
# - Check for all other draws

@cache
def Generate_White_Pawn_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    # Loop over all white pawns in the WhitePawns bitboard
    while board_copy:

        Source, Index = Get_LSB_and_Index(board_copy) 

        # Normal Moves
        # -----------------------------------------------------------------------
        MoveList.extend( Pawn_Normal_Moves(Game, Source, 'w') )

        # Attacking Moves
        # -----------------------------------------------------------------------
        MoveList.extend( Pawn_Atk_Moves(Game, Source, 'w') ) 

        # Special Moves
        # -----------------------------------------------------------------------
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if BLACK_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:
                MoveList.append( Move('w',Index,Square_to_index[Game.En_Passant],True,False,'P',False,True) )

        # Remove this bit from the board
        board_copy ^= Source

    return MoveList

@cache
def Generate_Black_Pawn_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    # Loop over black pawns in the BlackPawns bitboard
    while board_copy:
        
        Source, Index = Get_LSB_and_Index(board_copy)

        # Normal Moves
        # -----------------------------------------------------------------------
        MoveList.extend( Pawn_Normal_Moves(Game, Source, 'b') )

        # Attacking Moves
        # -----------------------------------------------------------------------
        MoveList.extend( Pawn_Atk_Moves(Game, Source, 'b') )

        # Special Moves
        # -----------------------------------------------------------------------
        if Game.En_Passant != None:

            # Get index of en passant square and check if a pawn can capture it
            if WHITE_PAWN_ATKS[ Square_to_index[Game.En_Passant] ] & Source:
                MoveList.append( Move('b',Index,Square_to_index[Game.En_Passant],True,False,'p',False,True) )    
    
        # Remove this bit from the board
        board_copy ^= Source

    return MoveList

def Generate_White_Knight_Moves(board_copy:i64, Game:GameState) -> list: 
    return Gen_Pseudo_legal_Moves(Game,board_copy,'N')
    
def Generate_Black_Knight_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'n')

def Generate_White_Bishop_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'B')

def Generate_Black_Bishop_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'b')

def Generate_White_Rook_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'R')

def Generate_Black_Rook_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'r')

def Generate_White_Queen_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'Q')

def Generate_Black_Queen_Moves(board_copy:i64, Game:GameState) -> list:
    return Gen_Pseudo_legal_Moves(Game,board_copy,'q')

@cache
def Generate_White_King_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    Source   = board_copy
    Index    = Board_To_Square(Source)

    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------
    IllegalSquares = All_Attacked_squares('b', Game, Source)

    # If the attacked squares includes the location of our king then the removing of illegal moves will fail
    IllegalSquares = Delete_bit(IllegalSquares, Index)

    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & IllegalSquares) | (Target & Game.WhiteAll)

    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'w','K') )

    # Castling Moves
    # ---------------------------------------------------------------------------
    
    # Castles can only take place on E1
    if Source == SquareE1:
        KingRightCastle   = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle    = (Source << i64(1)) | (Source << i64(2)) | (Source << i64(3))
        KingLeftAtkVerify = (Source << i64(1)) | (Source << i64(2)) | (Source)
        AttackedSquares   = All_Attacked_squares('b',Game)

        # Validate kingside castle rights      ⇒ Verify there are no obstructions ⇒
        # Ensure these squares aren't attacked ⇒ Ensure there is no check
        if ( (Game.Castle_Rights & W_KingSide) and (not Is_Obstructed(Game,KingRightCastle)) and
             (not(AttackedSquares & KingRightCastle)) and (Game.WhiteCheckMask == AllBits) ):
       
            MoveList.append( Move('w',Index,Index - 2,False,True,'K',False) )

        # Validate queenside castle rights     ⇒ Verify there are no obstructions ⇒
        # Ensure these squares aren't attacked ⇒ Ensure there is no check
        if ( (Game.Castle_Rights & W_QueenSide) and (not Is_Obstructed(Game,KingLeftCastle)) and
             (not(AttackedSquares & KingLeftAtkVerify)) ):
            
            MoveList.append( Move('w',Index,Index + 2,False,True,'K',False) )

    return MoveList

@cache
def Generate_Black_King_Moves(board_copy:i64, Game:GameState) -> list:
    MoveList = []
    Source   = board_copy
    Index    = Board_To_Square(Source)

    Target = KING_MOVES[Index]

    # Non-Castling Moves
    # ---------------------------------------------------------------------------
    IllegalSquares = All_Attacked_squares('w', Game, Source)
    
    # If the attacked squares includes the location of our king then the removing of illegal moves will fail
    IllegalSquares = Delete_bit(IllegalSquares, Index)
    
    # Filter out illegal moves (can't move into check / cant capture your own piece)
    Target ^= (Target & IllegalSquares) | (Target & Game.BlackAll)

    MoveList.extend( Create_Moves_From_Board(Game,Source,Target,'b','k') )

    # Castling Moves
    # ---------------------------------------------------------------------------
    
    # Castles can only take place on E8
    if Source == SquareE8:
        KingRightCastle   = (Source >> i64(1)) | (Source >> i64(2)) 
        KingLeftCastle    = (Source << i64(1)) | (Source << i64(2)) | (Source << i64(3))
        KingLeftAtkVerify = (Source << i64(1)) | (Source << i64(2)) | (Source)
        AttackedSquares   = All_Attacked_squares('w',Game)

        # Validate kingside castle rights      ⇒ Verify there are no obstructions ⇒
        # Ensure these squares aren't attacked ⇒ Ensure there is no check
        if ( (Game.Castle_Rights & B_KingSide) and (not Is_Obstructed(Game,KingRightCastle)) and
             (not(AttackedSquares & KingRightCastle)) and (Game.BlackCheckMask == AllBits) ):

            MoveList.append( Move('b',Index,Index - 2,False,True,'k',False) )

        # Validate queenside castle rights     ⇒ Verify there are no obstructions ⇒
        # Ensure these squares aren't attacked ⇒ Ensure there is no check
        if ( (Game.Castle_Rights & B_QueenSide) and (not Is_Obstructed(Game,KingLeftCastle)) and
             (not(AttackedSquares & KingLeftAtkVerify)) ):

            MoveList.append( Move('b',Index,Index + 2,False,True,'k',False) )

    return MoveList

@cache
def Generate_Moves(Game:GameState, col:str) -> list:
    MoveList = []
    Check = Game.WhiteCheckMask if (col == 'w') else Game.BlackCheckMask
    
    # If there is a double check only the king may move
    if Check == 'Double':
        if col == 'w': return Generate_White_King_Moves(Game.WhiteKing, Game)
        else:          return Generate_Black_King_Moves(Game.BlackKing, Game)

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

    return Filter_All_Moves(Game, MoveList)
