#/********************************************\
#      C O N S T A N T S  &  T A B L E S
#      - - - - - - - - -  -  - - - - - -
#\********************************************/
import math
import numpy as np

# Shorthand
i64 = np.uint64
i8  = np.uint8

# Ignores warnings generated by hyperbola quintessence and other functions
np.seterr(over='ignore')

# Keep in mind the board is reverse little endian
# That means the squares map to the board like this:
'''
     A    B    C    D    E    F    G    H
   +----+----+----+----+----+----+----+----+
 8 | 63 | 62 | 61 | 60 | 59 | 58 | 57 | 56 |  8th rank
   +----+----+----+----+----+----+----+----+
 7 | 55 | 54 | 53 | 52 | 51 | 50 | 49 | 48 |  7th rank
   +----+----+----+----+----+----+----+----+
 6 | 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 |  6th rank
   +----+----+----+----+----+----+----+----+
 5 | 39 | 38 | 37 | 36 | 35 | 34 | 33 | 32 |  5th rank
   +----+----+----+----+----+----+----+----+
 4 | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 24 |  4th rank
   +----+----+----+----+----+----+----+----+
 3 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 |  3rd rank
   +----+----+----+----+----+----+----+----+
 2 | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  2nd rank
   +----+----+----+----+----+----+----+----+
 1 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  1st rank
   +----+----+----+----+----+----+----+----+
     A    B    C    D    E    F    G    H - file(s)

--------------------------------------------------------
credits = https://www.chessprogramming.org/Little-endian
'''

# Reverses a set of bits e.g. 1011 -> 1101 (Used for hyperbols quintessence)
# https://chess.stackexchange.com/questions/37309/move-generation-for-sliding-pieces-and-hyperbola-quintessence
def Reverse_bits(b:i64) -> i64:
    b = (b & i64(0x5555555555555555)) << i64(1) | ((b >> i64(1)) & i64(0x5555555555555555))
    b = (b & i64(0x3333333333333333)) << i64(2) | ((b >> i64(2)) & i64(0x3333333333333333))
    b = (b & i64(0x0f0f0f0f0f0f0f0f)) << i64(4) | ((b >> i64(4)) & i64(0x0f0f0f0f0f0f0f0f))
    b = (b & i64(0x00ff00ff00ff00ff)) << i64(8) | ((b >> i64(8)) & i64(0x00ff00ff00ff00ff))

    return (b << i64(48)) | ((b & i64(0xffff0000)) << i64(16)) | ((b >> i64(16)) & i64(0xffff0000)) | (b >> i64(48))

#/*******************************************\
#              B I T B O A R D S       
#              - - - - - - - - -
#\*******************************************/

# Helper bitboard with all bits as 1
AllBits = i64(0xffffffffffffffff)

# Helper bitboard with all bits as 0
NoBits = i64(0)

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
SquareH1 = i64(1)
SquareF1 = i64(4)
SquareH8 = i64(2**56)
SquareF8 = i64(2**58)
SquareA1 = i64(2**7)
SquareD1 = i64(2**4)
SquareA8 = i64(2**63)
SquareD8 = i64(2**60)

# Dictionary that takes string of a square's name and gives its index ('e4') -> 36
Square_to_index = {
    'h1': 0,  'g1': 1,  'f1': 2,  'e1': 3,  'd1': 4,  'c1': 5,  'b1': 6,  'a1': 7, 
    'h2': 8,  'g2': 9,  'f2': 10, 'e2': 11, 'd2': 12, 'c2': 13, 'b2': 14, 'a2': 15, 
    'h3': 16, 'g3': 17, 'f3': 18, 'e3': 19, 'd3': 20, 'c3': 21, 'b3': 22, 'a3': 23, 
    'h4': 24, 'g4': 25, 'f4': 26, 'e4': 27, 'd4': 28, 'c4': 29, 'b4': 30, 'a4': 31, 
    'h5': 32, 'g5': 33, 'f5': 34, 'e5': 35, 'd5': 36, 'c5': 37, 'b5': 38, 'a5': 39, 
    'h6': 40, 'g6': 41, 'f6': 42, 'e6': 43, 'd6': 44, 'c6': 45, 'b6': 46, 'a6': 47, 
    'h7': 48, 'g7': 49, 'f7': 50, 'e7': 51, 'd7': 52, 'c7': 53, 'b7': 54, 'a7': 55, 
    'h8': 56, 'g8': 57, 'f8': 58, 'e8': 59, 'd8': 60, 'c8': 61, 'b8': 62, 'a8': 63 }

# Inverse of Sqaure to index, take an index and return the square's name 36 -> ('e4')
Index_to_square = {
    0: 'h1', 1: 'g1', 2: 'f1', 3: 'e1', 4: 'd1', 5: 'c1', 6: 'b1', 7: 'a1',
    8: 'h2', 9: 'g2', 10: 'f2', 11: 'e2', 12: 'd2', 13: 'c2', 14: 'b2', 15: 'a2',
    16: 'h3', 17: 'g3', 18: 'f3', 19: 'e3', 20: 'd3', 21: 'c3', 22: 'b3', 23: 'a3',
    24: 'h4', 25: 'g4', 26: 'f4', 27: 'e4', 28: 'd4', 29: 'c4', 30: 'b4', 31: 'a4', 
    32: 'h5', 33: 'g5', 34: 'f5', 35: 'e5', 36: 'd5', 37: 'c5', 38: 'b5', 39: 'a5', 
    40: 'h6', 41: 'g6', 42: 'f6', 43: 'e6', 44: 'd6', 45: 'c6', 46: 'b6', 47: 'a6', 
    48: 'h7', 49: 'g7', 50: 'f7', 51: 'e7', 52: 'd7', 53: 'c7', 54: 'b7', 55: 'a7', 
    56: 'h8', 57: 'g8', 58: 'f8', 59: 'e8', 60: 'd8', 61: 'c8', 62: 'b8', 63: 'a8' }

# Castling indicators
# Castles are encoded as following: - White controls the 4 and 8 bits
#                                   - Black controls the 1 and 2 bits
# Kingside is the rightmost bit, Queenside is the leftmost bit
W_KingSide  = i8( 0b0100 )
W_QueenSide = i8( 0b1000 )
B_KingSide  = i8( 0b0001 )
B_QueenSide = i8( 0b0010 )

#/*******************************************\
#       P R E - C O M P U T A T I O N S      
#       - - -   - - - - - - - - - - - -
#\*******************************************/

# Sliding Pieces
# -------------------------------------------------------------------------------

# Implementation of hyperbola quintessence for sliding move generation    
# https://www.chessprogramming.org/Hyperbola_Quintessence
def Sliding_Moves(Location:i64, blockers:i64, mask:i64) -> i64:    
    o = blockers & i64(mask)
    r = Reverse_bits(o)
    o -= Location
    r -= Reverse_bits(Location)
    o ^= Reverse_bits(r)
    o &= mask

    return o

def Compute_Rook_attacks(RookLocation:i64, blockers:i64) -> i64:
    # Piece bitboards are in the form 2^n, to find n we take log base 2 of this bitboard
    SquareNum = int( np.log2(RookLocation) )
    Rank = (SquareNum // 8) + 1 # Calculates rank index
    File = SquareNum % 8        # Calculates file index
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

# Arrays
# -------------------------------------------------------------------------------

# Precomputing all movement arrays, the 'Compute_*(non-slider piece here)*_Moves()' functions
# can be viewed in the previous commits (before the 4th Jan 2024), they have been removed because they only need
# to be used one time, in order to calculate the following arrays.

# The positions varible is simply a list of all square indexes 0..63 and can be expressed as 'list(range(0,64))'

# Precomputed with the following expression:
# 'np.array( [Compute_King_Moves( i64(2**pos) ) for pos in positions] )'
KING_MOVES = np.array([i64(0x302), i64(0x705), i64(0xe0a), i64(0x1c14), i64(0x3828), 
    i64(0x7050), i64(0xe0a0), i64(0xc040), i64(0x30203), i64(0x70507), 
    i64(0xe0a0e), i64(0x1c141c), i64(0x382838), i64(0x705070), i64(0xe0a0e0),
    i64(0xc040c0), i64(0x3020300), i64(0x7050700), i64(0xe0a0e00),
    i64(0x1c141c00), i64(0x38283800), i64(0x70507000), i64(0xe0a0e000), 
    i64(0xc040c000), i64(0x302030000), i64(0x705070000), i64(0xe0a0e0000), 
    i64(0x1c141c0000), i64(0x3828380000), i64(0x7050700000), 
    i64(0xe0a0e00000), i64(0xc040c00000), i64(0x30203000000), 
    i64(0x70507000000), i64(0xe0a0e000000), i64(0x1c141c000000), 
    i64(0x382838000000), i64(0x705070000000), i64(0xe0a0e0000000),
    i64(0xc040c0000000), i64(0x3020300000000), i64(0x7050700000000),
    i64(0xe0a0e00000000), i64(0x1c141c00000000), i64(0x38283800000000),
    i64(0x70507000000000), i64(0xe0a0e000000000), i64(0xc040c000000000),
    i64(0x302030000000000), i64(0x705070000000000), i64(0xe0a0e0000000000), 
    i64(0x1c141c0000000000), i64(0x3828380000000000), i64(0x7050700000000000),
    i64(0xe0a0e00000000000), i64(0xc040c00000000000), i64(0x203000000000000),
    i64(0x507000000000000), i64(0xa0e000000000000), i64(0x141c000000000000), 
    i64(0x2838000000000000), i64(0x5070000000000000), i64(0xa0e0000000000000), i64(0x40c0000000000000)] )

# Precomputed with the following expression:
# 'np.array( [Compute_Knight_Moves( i64(2**pos) ) for pos in positions] )'
KNIGHT_MOVES = np.array([i64(0x20400), i64(0x50800), i64(0xa1100), i64(0x142200), i64(0x284400),
    i64(0x508800), i64(0xa01000), i64(0x402000), i64(0x2040004), i64(0x5080008), 
    i64(0xa110011), i64(0x14220022), i64(0x28440044), i64(0x50880088), i64(0xa0100010),
    i64(0x40200020), i64(0x204000402), i64(0x508000805), i64(0xa1100110a),
    i64(0x1422002214), i64(0x2844004428), i64(0x5088008850), i64(0xa0100010a0),
    i64(0x4020002040), i64(0x20400040200), i64(0x50800080500), i64(0xa1100110a00),
    i64(0x142200221400), i64(0x284400442800), i64(0x508800885000), i64(0xa0100010a000),
    i64(0x402000204000), i64(0x2040004020000), i64(0x5080008050000),
    i64(0xa1100110a0000), i64(0x14220022140000), i64(0x28440044280000), 
    i64(0x50880088500000), i64(0xa0100010a00000), i64(0x40200020400000), 
    i64(0x204000402000000), i64(0x508000805000000), i64(0xa1100110a000000), 
    i64(0x1422002214000000), i64(0x2844004428000000), i64(0x5088008850000000), 
    i64(0xa0100010a0000000), i64(0x4020002040000000), i64(0x400040200000000), 
    i64(0x800080500000000), i64(0x1100110a00000000), i64(0x2200221400000000),
    i64(0x4400442800000000), i64(0x8800885000000000), i64(0x100010a000000000), 
    i64(0x2000204000000000), i64(0x4020000000000), i64(0x8050000000000),
    i64(0x110a0000000000), i64(0x22140000000000), i64(0x44280000000000), 
    i64(0x88500000000000), i64(0x10a00000000000), i64(0x20400000000000)] )

# Precomputed with the following expression:
# 'np.array( [Compute_Pawn_Moves( i64(2**pos), 'w' ) for pos in positions] )'
WHITE_PAWN_MOVES = np.array([i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0),
    i64(0x0), i64(0x0), i64(0x0), i64(0x1010000), i64(0x2020000),i64(0x4040000),
    i64(0x8080000), i64(0x10100000), i64(0x20200000), i64(0x40400000), i64(0x80800000),
    i64(0x1000000), i64(0x2000000), i64(0x4000000), i64(0x8000000), i64(0x10000000), 
    i64(0x20000000), i64(0x40000000), i64(0x80000000), i64(0x100000000), i64(0x200000000), 
    i64(0x400000000), i64(0x800000000), i64(0x1000000000), i64(0x2000000000), i64(0x4000000000), 
    i64(0x8000000000), i64(0x10000000000), i64(0x20000000000), i64(0x40000000000), i64(0x80000000000), 
    i64(0x100000000000), i64(0x200000000000),i64(0x400000000000), i64(0x800000000000),
    i64(0x1000000000000), i64(0x2000000000000), i64(0x4000000000000), i64(0x8000000000000), 
    i64(0x10000000000000), i64(0x20000000000000), i64(0x40000000000000), i64(0x80000000000000), 
    i64(0x100000000000000), i64(0x200000000000000), i64(0x400000000000000), i64(0x800000000000000), 
    i64(0x1000000000000000), i64(0x2000000000000000), i64(0x4000000000000000), i64(0x8000000000000000), 
    i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0)] )

# Precomputed with the following expression:
# 'np.array( [Compute_Pawn_Moves( i64(2**pos), 'b' ) for pos in positions] )'
BLACK_PAWN_MOVES = np.array([i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), 
    i64(0x0), i64(0x0), i64(0x0), i64(0x1), i64(0x2), i64(0x4), 
    i64(0x8), i64(0x10), i64(0x20), i64(0x40), i64(0x80), i64(0x100),
    i64(0x200), i64(0x400), i64(0x800), i64(0x1000), i64(0x2000), i64(0x4000), 
    i64(0x8000), i64(0x10000), i64(0x20000), i64(0x40000), i64(0x80000),
    i64(0x100000), i64(0x200000), i64(0x400000), i64(0x800000), i64(0x1000000),
    i64(0x2000000), i64(0x4000000), i64(0x8000000), i64(0x10000000), i64(0x20000000), 
    i64(0x40000000), i64(0x80000000), i64(0x100000000), i64(0x200000000), i64(0x400000000), 
    i64(0x800000000), i64(0x1000000000), i64(0x2000000000), i64(0x4000000000),
    i64(0x8000000000), i64(0x10100000000), i64(0x20200000000), i64(0x40400000000),
    i64(0x80800000000), i64(0x101000000000), i64(0x202000000000), i64(0x404000000000), 
    i64(0x808000000000), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0)] )

# Precomputed with the following expression:
# 'np.array( [Compute_Pawn_attacks( i64(2**pos), 'w') for pos in positions] )'
WHITE_PAWN_ATKS = np.array([i64(0x200), i64(0x500), i64(0xa00), i64(0x1400), i64(0x2800), 
    i64(0x5000), i64(0xa000), i64(0x4000), i64(0x20000), i64(0x50000), i64(0xa0000), 
    i64(0x140000), i64(0x280000), i64(0x500000), i64(0xa00000), i64(0x400000), i64(0x2000000), 
    i64(0x5000000), i64(0xa000000), i64(0x14000000), i64(0x28000000), i64(0x50000000), i64(0xa0000000), 
    i64(0x40000000), i64(0x200000000), i64(0x500000000), i64(0xa00000000), i64(0x1400000000), i64(0x2800000000), 
    i64(0x5000000000), i64(0xa000000000), i64(0x4000000000), i64(0x20000000000), i64(0x50000000000), i64(0xa0000000000), 
    i64(0x140000000000), i64(0x280000000000), i64(0x500000000000), i64(0xa00000000000), i64(0x400000000000), 
    i64(0x2000000000000), i64(0x5000000000000), i64(0xa000000000000), i64(0x14000000000000), i64(0x28000000000000), 
    i64(0x50000000000000), i64(0xa0000000000000), i64(0x40000000000000), i64(0x200000000000000), i64(0x500000000000000), 
    i64(0xa00000000000000), i64(0x1400000000000000), i64(0x2800000000000000), i64(0x5000000000000000), i64(0xa000000000000000),
    i64(0x4000000000000000), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0)] )

# Precomputed with the following expression:
# 'np.array( [Compute_Pawn_attacks( i64(2**pos),'b' ) for pos in positions] )'
BLACK_PAWN_ATKS = np.array( [i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), i64(0x0), 
    i64(0x2), i64(0x5), i64(0xa), i64(0x14), i64(0x28), i64(0x50), i64(0xa0), i64(0x40), i64(0x200), 
    i64(0x500), i64(0xa00), i64(0x1400), i64(0x2800), i64(0x5000), i64(0xa000), i64(0x4000), i64(0x20000), 
    i64(0x50000), i64(0xa0000), i64(0x140000), i64(0x280000), i64(0x500000), i64(0xa00000), i64(0x400000), 
    i64(0x2000000), i64(0x5000000), i64(0xa000000), i64(0x14000000), i64(0x28000000), i64(0x50000000), 
    i64(0xa0000000), i64(0x40000000), i64(0x200000000), i64(0x500000000), i64(0xa00000000), i64(0x1400000000), 
    i64(0x2800000000), i64(0x5000000000), i64(0xa000000000), i64(0x4000000000), i64(0x20000000000), i64(0x50000000000), 
    i64(0xa0000000000), i64(0x140000000000), i64(0x280000000000), i64(0x500000000000), i64(0xa00000000000), i64(0x400000000000),
    i64(0x2000000000000), i64(0x5000000000000), i64(0xa000000000000), i64(0x14000000000000), i64(0x28000000000000), 
    i64(0x50000000000000), i64(0xa0000000000000), i64(0x40000000000000)] )
