#/********************************************\
#             B I T   M A C R O S      
#             - - -   - - - - - - 
#\********************************************/
from textwrap           import wrap
from ConstantsAndTables import *
import numpy as np
from math import log2

# Shorthands
i64 = np.uint64
i8  = np.uint8  

# Returns 0 if a bitboard hasn't got this bit as a one
def Get_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard & i64(2**SquareNum)

# Sets a bit on bitboard, 0 to 1
def Set_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard | i64(2**SquareNum)

# Removes a bit from bitboard, 1 to 0
def Delete_bit(Bitboard:i64, SquareNum:int) -> i64: return Bitboard ^ i64(2**SquareNum)

# Gets the least significant bit (bit worth the lowest value)
def Get_LSB(Bitboard:i64) -> i64 | int:
    if Bitboard:return Bitboard & (~Bitboard + i64(1))
    # No bits on bitboard so return -1
    return -1

# Checks if given bitboard resides on the 2nd rank (used to validate pawn moves)
def Is_Second_Rank(bitboard:i64) -> i64: return bitboard & Rank2

# Checks if given bitboard resides on the 1st rank (used to validate pawn moves)
def Is_First_Rank(bitboard:i64) -> i64: return bitboard & Rank1

# Checks if given bitboard resides on the 7th rank (used to validate pawn moves)
def Is_Seventh_Rank(bitboard:i64) -> i64: return bitboard & Rank7

# Checks if given bitboard resides on the 8th rank (used to validate pawn moves)
def Is_Eigth_Rank(bitboard:i64) -> i64: return bitboard & Rank8

# Returns both the least significant bit and the index from (0 - 63)
def Get_LSB_and_Index(bitboard:i64) -> tuple[i64,int]:
    LSB = Get_LSB(bitboard)
    return LSB, GetIndex( LSB )

# Returns index of the one bit on the given bitboard
def GetIndex(bitboard:i64) -> int: return int( math.log2(bitboard) )

# Checks whether the given board has bits within the mask
def In_Mask(bitboard:i64, mask:i64) -> i64: return bitboard & mask 

# Function to count how many bits are in some binary representation of a number
def BitCount(n:i64) -> int:
    count = 0
    while n:

        # Removes last bit
        n &= ( n-i64(1) )

        # Increment count by one until no more one bits left
        count += 1

    return count

# Nice way to visualise the board
def Show_bitboard(bb:i64) -> str:
    # Fills the binary with extra zeros until 64 digits, (8x8 board)  
    result = str(bin(bb)[2:]).zfill(64)
    print(result)               
    return '\n'.join([' '.join(wrap(line, 1)) for line in wrap(result, 8)])

# Because single-bit bitboards can be expressed in the form 2^n, where n is the square number
# we can get n simply by taking logarithm base 2 of the bitboard given
def Board_To_Square(bb:i64) -> int: return int(log2(bb))

# Builds an attacking ray between 2 squares if the 2 squares are in line 
# horizontally, vertically or diagonally (Used in Is_Check function)
# could potentially be precomputed should only be 64^2 of mem
def Build_Ray(Square1:i64, Square2:i64) -> i64:

    # Empty board for generating ray attacks without obstructions
    Empty = i64(0)

    # The bits where 2 rays intersect will be kept by the 'AND (&)' and everything else will become 0
    Ray = Compute_Queen_attacks( Square1,Empty ) & Compute_Queen_attacks( Square2,Empty )

    # Add both source squares to the ray
    Ray |= (Square1 | Square2)

    return Ray
