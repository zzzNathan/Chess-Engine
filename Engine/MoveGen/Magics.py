# References:
# ------------
# - [1] https://talkchess.com/forum3/viewtopic.php?p=951355&hilit=magic+bitboards+explained#p951355
# - [2] https://github.com/mvanthoor/rustic/blob/master/src/extra/wizardry.rs
# - [3] http://pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf
# - [4] https://www.youtube.com/watch?v=_vqlIPDR2TU&t=2450s
# - [5] https://analog-hors.github.io/site/magic-bitboards/
# - [6] https://www.chessprogramming.org/Traversing_Subsets_of_a_Set#All_Subsets_of_any_Set
# 
# I aim to heavily explain/document this code seeing as learning then 
# creating an implementation of magic bitboards is not at all an easy feat,
# Not only that but there is also a huge lack of resources to learn about
# this technique. Inspired by the Rustic chess engine.

# We aim to have to tables of 64 elements each representing one square 
# on our chess board. In each table we would like to be able to hash
# the bitboard of current blockers on the board and recieve as output 
# the bitboard containing legal moves in this specific blocker configuration.

# One table will be for rook moves and the other 
# will be for bishop moves. Queen moves can be found using a combination
# of both rook and bishop moves.

# For each square we would like to have an object with two attributes
# The first will be the magic number for this square, the second
# will be the shift for this square.
from collections.abc import Generator
import numpy as np
from Engine.Utils.Constants import AllBits
i64 = np.uint64

# BB or bb refers to Bitboard throughout the code

# Function to determine whether we can travel in this direction legally
# square -> An integer representing the index of the square we would to move to.
# delta  -> The integer direction we would like to move in.
def Legal(square:int, delta:int) -> bool:
    RIGHT_BOUNDARY = [i for i in range(0, 57, 8)] # List of squares that lie on the right boundary of board
    LEFT_BOUNDARY  = [i for i in range(7, 63, 8)] # List of squares that lie on the left boundary of board
    UP_BOUNDARY    = [i for i in range(56, 64)]   # List of squares that lie on the top boundary of board
    DOWN_BOUNDARY  = [i for i in range(0, 8)]     # List of squares that lie on the bottom boundary of board

    # Can't move right on right boundary
    if (delta == -1 and square in RIGHT_BOUNDARY): return False # Right
    if (delta == 7  and square in RIGHT_BOUNDARY): return False # Diag-up-right
    if (delta == -9 and square in RIGHT_BOUNDARY): return False # Diag-down-right

    # Can't move left on left boundary
    if (delta == 1  and square in LEFT_BOUNDARY): return False # Left
    if (delta == 9  and square in LEFT_BOUNDARY): return False # Diag-up-left
    if (delta == -7 and square in LEFT_BOUNDARY): return False # Diag-down-left

    # Can't move up on up boundary
    if (delta == 8  and square in UP_BOUNDARY): return False # Up
    if (delta == 7  and square in UP_BOUNDARY): return False # Diag-up-right
    if (delta == 9  and square in UP_BOUNDARY): return False # Diag-up-left

    # Can't move down on down boundary
    if (delta == -8  and square in DOWN_BOUNDARY): return False # Down
    if (delta == -9  and square in DOWN_BOUNDARY): return False # Diag-down-right
    if (delta == -7  and square in DOWN_BOUNDARY): return False # Diag-down-left
    return True

# Gets moves from a given square to use as a mask for when we want to index into
# our magic tables.
# sq   -> An integer representing the index of the square we would like to get moves from.
# rook -> A boolean flag to show if we are generating moves for the rook or not.
def Get_Moves(sq:int, rook:bool) -> i64:
    MoveBB = i64(0) # Bitboard containing moves
    # Directions we are able to travel in (see Utils/Constants.py) 
    rook_dirs   = [8, -8, 1, -1] # In order: Up, Down, Left, Right 
    bishop_dirs = [7, 9, -9, -7] # In order: Diag-up-right, Diag-up-left, Diag-down-right, Diag-down-left

    Directions = rook_dirs if (rook) else bishop_dirs
    
    # Iterate over all possible directions ..
    for delta in Directions:
        sq_curr = sq
        
        # We can skip this direction if it leads us off the board immediately
        if not (0 <= sq_curr + delta <= 63): continue

        # Until we reach the edge of the board
        while 1:
            # Move one step in the new direction
            sq_curr += delta
            
            # If sqaure is on the board .. 
            if Legal(sq_curr, delta):
                # Add bit to our move bitboard
                sq_bitboard = i64(2**sq_curr)
                MoveBB |= sq_bitboard
            
            # Otherwise we have reached end of the board, we need not consider
            # the last square on the boundary, since this will have no impact
            # on our move generation. (See reference [4] timestamp: 29:00 - 34:00)
            # for a good explanation on why.
            else: break

    return MoveBB

# Generator function to get all possible blockers from a given mask
# mask -> A bitboard representing all the squares for which we will generate blockers
def Get_All_Blockers(mask:i64) -> Generator:
    # We will make use of the "Carry-rippler" technique in order to generate 
    # all possible blocker configurations from the given mask 
    # (See reference [5] and [6])
    subset = i64(0)
    subset = (subset - mask) & mask

    while subset:
        yield subset
        subset = (subset - mask) & mask

# 64 bit random number generator to find candidate magic numbers
def Gen_Random64() -> i64:
    r1 = np.random.randint(0, AllBits, dtype=i64)
    r2 = np.random.randint(0, AllBits, dtype=i64)
    r3 = np.random.randint(0, AllBits, dtype=i64)
    return i64(r1 & r2 & r3)

# Function to find magics with trial and error
# rook -> A boolean flag to show if we are generating moves for the rook or not.
def Find_Magics(rook:bool) -> None:
