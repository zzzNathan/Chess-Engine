#/*******************************************\
#      A U T H O R: Jonathan Kasongo
#      - - - - - -  - - - - - - - - 
#               Y E A R: 2023
#               - - - -  - - -
#\*******************************************/
import math
import numpy as np

# Shorthand
i64 = np.uint64
# Ignores warnings generated by hyperbola quintessence
np.seterr(over='ignore')

# Reverses a set of bits e.g. 1011 -> 1101
# https://chess.stackexchange.com/questions/37309/move-generation-for-sliding-pieces-and-hyperbola-quintessence
def Reverse_bits(b:i64) -> i64:
    b = (b & i64(0x5555555555555555)) << i64(1) | ((b >> i64(1)) & i64(0x5555555555555555))
    b = (b & i64(0x3333333333333333)) << i64(2) | ((b >> i64(2)) & i64(0x3333333333333333))
    b = (b & i64(0x0f0f0f0f0f0f0f0f)) << i64(4) | ((b >> i64(4)) & i64(0x0f0f0f0f0f0f0f0f))
    b = (b & i64(0x00ff00ff00ff00ff)) << i64(8) | ((b >> i64(8)) & i64(0x00ff00ff00ff00ff))

    return (b << i64(48)) | ((b & i64(0xffff0000)) << i64(16)) | ((b >> i64(16)) & i64(0xffff0000)) | (b >> i64(48))

# Uppercase will signify a white piece, lowercase will signify a black piece
PIECE_TO_ASCII = {'WPawn':'P', 'BPawn':'p', 'WKnight':'N', 'BKnight':'n', 'WBishop':'B', 'BBishop':'b',
                  'WRook':'R', 'BRook':'r', 'WQueen':'Q', 'BQueen':'q', 'WKing':'K', 'BKing':'k',}

#/*******************************************\
#              B I T B O A R D S       
#              - - - - - - - - -
#\*******************************************/

# Helper bitboard with all bits as 1
AllBits = i64(0xffffffffffffffff)

# File mask initialisation
NotFileA = i64(0x7F7F7F7F7F7F7F7F)
FileA    = i64(0x8080808080808080)
NotFileB = i64(0xBFBFBFBFBFBFBFBF)
FileB    = i64(0x4040404040404040)
FileC    = i64(0x2020202020202020)
FileD    = i64(0x1010101010101010)
FileE    = i64(0x808080808080808)
FileF    = i64(0x404040404040404)
NotFileG = i64(0xFDFDFDFDFDFDFDFD)
FileG    = i64(0x202020202020202)
NotFileH = i64(0xFEFEFEFEFEFEFEFE)
FileH    = i64(0x101010101010101)
FILES    = [FileH,FileG,FileF,FileE,FileD,FileC,FileB,FileA]

# Rank mask initialisation
Rank8 = i64(0xFF00000000000000)
Rank7 = i64(0xFF000000000000)
Rank6 = i64(0xFF0000000000)
Rank5 = i64(0xFF00000000)
Rank4 = i64(0xFF000000)
Rank3 = i64(0xFF0000)
Rank2 = i64(0xFF00)
Rank1 = i64(0xFF)
RANKS = [-1,Rank1,Rank2,Rank3,Rank4,Rank5,Rank6,Rank7,Rank8]

# Diagonal mask initialisation (bottom-left to top-right)
Diag_H1_H1 = i64(0b1)
Diag_G1_H2 = (Diag_H1_H1<<i64(1)) | i64(2**8)
Diag_F1_H3 = (Diag_G1_H2<<i64(1)) | i64(2**16)
Diag_E1_H4 = (Diag_F1_H3<<i64(1)) | i64(2**24)
Diag_D1_H5 = (Diag_E1_H4<<i64(1)) | i64(2**32)
Diag_C1_H6 = (Diag_D1_H5<<i64(1)) | i64(2**40)
Diag_B1_H7 = (Diag_C1_H6<<i64(1)) | i64(2**48)
Diag_A1_H8 = (Diag_B1_H7<<i64(1)) | i64(2**56)
Diag_A2_G8 = (Diag_A1_H8<<i64(1)) ^ i64(2**8) # XOR's remove the extra bits
Diag_A3_F8 = (Diag_A2_G8<<i64(1)) ^ i64(2**16)# that appear after the left bit shift
Diag_A4_E8 = (Diag_A3_F8<<i64(1)) ^ i64(2**24)
Diag_A5_D8 = (Diag_A4_E8<<i64(1)) ^ i64(2**32)
Diag_A6_C8 = (Diag_A5_D8<<i64(1)) ^ i64(2**40)
Diag_A7_B8 = (Diag_A6_C8<<i64(1)) ^ i64(2**48)
Diag_A8_A8 = (Diag_A7_B8<<i64(1)) ^ i64(2**56)
DIAGONALS = [  # Square number to diagonal masks
    Diag_H1_H1, Diag_G1_H2, Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8,
    Diag_G1_H2, Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8,
    Diag_F1_H3, Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8,
    Diag_E1_H4, Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, 
    Diag_D1_H5, Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8,
    Diag_C1_H6, Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8,
    Diag_B1_H7, Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8, Diag_A7_B8,
    Diag_A1_H8, Diag_A2_G8, Diag_A3_F8, Diag_A4_E8, Diag_A5_D8, Diag_A6_C8, Diag_A7_B8, Diag_A8_A8
]

# Anti-Diagonal mask initialisation (bottom-right to top left)
Diag_A1_A1 = i64(0b10000000)
Diag_B1_A2 = (Diag_A1_A1>>i64(1)) | i64(2**15)
Diag_C1_A3 = (Diag_B1_A2>>i64(1)) | i64(2**23)
Diag_D1_A4 = (Diag_C1_A3>>i64(1)) | i64(2**31)
Diag_E1_A5 = (Diag_D1_A4>>i64(1)) | i64(2**39)
Diag_F1_A6 = (Diag_E1_A5>>i64(1)) | i64(2**47)
Diag_G1_A7 = (Diag_F1_A6>>i64(1)) | i64(2**55)
Diag_H1_A8 = (Diag_G1_A7>>i64(1)) | i64(2**63)
Diag_H2_B8 = (Diag_H1_A8>>i64(1)) 
Diag_H3_C8 = (Diag_H2_B8>>i64(1)) ^ i64(2**7) # XOR'S remove the extra bits
Diag_H4_D8 = (Diag_H3_C8>>i64(1)) ^ i64(2**15)# that appear after the right bit shift
Diag_H5_E8 = (Diag_H4_D8>>i64(1)) ^ i64(2**23)
Diag_H6_F8 = (Diag_H5_E8>>i64(1)) ^ i64(2**31)
Diag_H7_G8 = (Diag_H6_F8>>i64(1)) ^ i64(2**39)
Diag_H8_H8 = (Diag_H7_G8>>i64(1)) ^ i64(2**47)
ANTI_DIAGONALS = [  # Square number to anti-diagonal masks
    Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3, Diag_B1_A2, Diag_A1_A1,
    Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3, Diag_B1_A2,
    Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4, Diag_C1_A3,
    Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5, Diag_D1_A4,
    Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6, Diag_E1_A5,
    Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7, Diag_F1_A6,
    Diag_H7_G8, Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8, Diag_G1_A7,
    Diag_H8_H8, Diag_H7_G8, Diag_H6_F8, Diag_H5_E8, Diag_H4_D8, Diag_H3_C8, Diag_H2_B8, Diag_H1_A8
]

# Specific square bitboards
SquareE1 = i64(8)
SquareE8 = i64(0x800000000000000)

#/*******************************************\
#       P R E - C O M P U T A T I O N S      
#       - - -   - - - - - - - - - - - -
#\*******************************************/

def Compute_King_Moves(KingLocation:i64) -> i64:
    Moves = i64(0)
    # North
    Moves |= KingLocation << i64(8)
    # North - East
    Moves |= (KingLocation << i64(7)) & NotFileA
    # North - West 
    Moves |= (KingLocation << i64(9)) & NotFileH
    # East
    Moves |= (KingLocation >> i64(1)) & NotFileA
    # West
    Moves |= (KingLocation << i64(1)) & NotFileH
    # South
    Moves |= (KingLocation >> i64(8))
    # South - East
    Moves |= (KingLocation >> i64(9)) & NotFileA
    # South - West 
    Moves |= (KingLocation >> i64(7)) & NotFileH

    return Moves 

def Compute_Knight_Moves(KnightLocation:i64) -> i64:
    Moves = i64(0)
    # North - East
    Moves |= (KnightLocation << i64(15)) & NotFileA
    # North - West
    Moves |= (KnightLocation << i64(17)) & NotFileH
    # East - North
    Moves |= (KnightLocation << i64(6)) & ~(FileA | FileB)
    # West - North
    Moves |= (KnightLocation << i64(10)) & ~(FileG | FileH)
    # East - South
    Moves |= (KnightLocation >> i64(10)) & ~(FileA | FileB)
    # West - South
    Moves |= (KnightLocation >> i64(6)) & ~(FileG | FileH)
    # South - East
    Moves |= (KnightLocation >> i64(17)) & NotFileA
    # South - West
    Moves |= (KnightLocation >> i64(15)) & NotFileH

    return Moves

def Compute_Pawn_Moves(PawnLocation:i64, colour:str)->i64:
    Moves = i64(0)
    if colour=='w':
        # Up one
        if i64(2**8) <= PawnLocation <= i64(2**55):
            Moves |= PawnLocation << i64(8) 
        # Double moves
        if i64(2**8) <= PawnLocation <= i64(2**15):
            Moves |= PawnLocation << i64(16)
    else:
        # Up one
        if i64(2**8) <= PawnLocation <= i64(2**55):
            Moves |= PawnLocation >> i64(8)
        # Double moves
        if i64(2**48) <= PawnLocation <= i64(2**55):
            Moves |= PawnLocation >> i64(16)
    return Moves

def Compute_Pawn_attacks(PawnLocation:i64, colour:str):
    Moves = i64(0)
    if colour == 'w':
        # Capture right
        Moves |= (PawnLocation << i64(7)) & NotFileA
        # Capture left
        Moves |= (PawnLocation << i64(9)) & NotFileH
    else:
        # Capture right (Black's left)
        Moves |= (PawnLocation >> i64(9)) & NotFileA
        # Capture left (Black's right)
        Moves |= (PawnLocation >> i64(7)) & NotFileH
    return Moves

# Implementation of hyperbola quintessence for sliding move generation    
# https://www.chessprogramming.org/Hyperbola_Quintessence
def Sliding_Moves(Location:i64, blockers:i64, mask:i64) -> i64:    
    o = blockers & mask
    r = Reverse_bits(o)
    o -= Location
    r -= Reverse_bits(Location)
    o ^= Reverse_bits(r)
    o &= mask

    return o

def Compute_Rook_attacks(RookLocation:i64, blockers:i64) -> i64:
    # Piece bitboards are in the form 2^n, to find n we take log base 2 of this bitboard
    SquareNum = int( math.log2(RookLocation) )
    Rank = (SquareNum // 8) + 1 # Calculates rank index
    File = SquareNum % 8 # Calculates file index
    return (Sliding_Moves(RookLocation, blockers, RANKS[Rank]) |
            Sliding_Moves(RookLocation, blockers, FILES[File]) )

def Compute_Bishop_attacks(BishopLocation:i64, blockers:i64)->i64:
    # Piece bitboards are in the form 2^n, to find n we take log base 2 of this bitboard
    SquareNum = int( math.log2(BishopLocation) )
    return (Sliding_Moves(BishopLocation, blockers, DIAGONALS[SquareNum]) |
            Sliding_Moves(BishopLocation, blockers, ANTI_DIAGONALS[SquareNum] ))

def Compute_Queen_attacks(QueenLocation:i64, blockers:i64)->i64:
    # Piece bitboards are in the form 2^n, to find n we take log base 2 of this bitboard
    SquareNum = int( math.log2(QueenLocation) )
    Rank = (SquareNum // 8) + 1
    File = SquareNum % 8

    return (Sliding_Moves(QueenLocation, blockers, RANKS[Rank]) |
            Sliding_Moves(QueenLocation, blockers, FILES[File]) |
            Sliding_Moves(QueenLocation, blockers, DIAGONALS[SquareNum]) |
            Sliding_Moves(QueenLocation, blockers, ANTI_DIAGONALS[SquareNum]))

# Indexes for all squares on a board
positions = list(range(0,64))

# Precomputing all movement arrays 
KING_MOVES       = np.array( [Compute_King_Moves( i64(2**pos) ) for pos in positions] )
KNIGHT_MOVES     = np.array( [Compute_Knight_Moves( i64(2**pos) ) for pos in positions] )
WHITE_PAWN_MOVES = np.array( [Compute_Pawn_Moves( i64(2**pos), 'w' ) for pos in positions] )
BLACK_PAWN_MOVES = np.array( [Compute_Pawn_Moves( i64(2**pos), 'b' ) for pos in positions] )
WHITE_PAWN_ATKS  = np.array( [Compute_Pawn_attacks( i64(2**pos), 'w') for pos in positions] )
BLACK_PAWN_ATKS  = np.array( [Compute_Pawn_attacks( i64(2**pos),'b' ) for pos in positions] )


'''
queenloc = i64(0b0000000000000000000000000000000000000000000000000000000000010000)
blockers = i64(0b1111111111111111000000000000000000000000000000001111111111111111)

from textwrap import wrap
# Nice way to visualise the board
def Show_bitboard(bb:int) -> str:
    # Fills the binary with extra zeros until 64 digits, (8x8 board)  
    result = str(bin(bb)[2:]).zfill(64)
    print(result)               
    return '\n'.join([' '.join(wrap(line, 1)) for line in wrap(result, 8)])

print(Show_bitboard(Compute_Queen_attacks(queenloc,blockers)))'''