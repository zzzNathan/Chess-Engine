#/*******************************************\
#   M O V E  G E N E R A T I O N  T E S T S        
#   - - - -  - - - - - - - - - -  - - - - -      
#\*******************************************/
# The following code aims to test if the file "MoveGenerator.py" generates legal, playable moves
from collections.abc import Iterator
from Engine.MoveGen.MoveGenerator import *
import chess
import chess.pgn
from os import path
from functools import cache
from sys import stdout

LOCAL_DIR = path.dirname(__file__)
TATA_STEEL_MASTERS_86TH = path.join(LOCAL_DIR ,r'PGN_Game_Files/tatamast24.pgn')
print = stdout.write

# An error to indicate wrong number of legal moves generated
class WrongNumberOfMoves(Exception): pass

# An error to indicate that illegal moves have been generated
class IllegalMovesGenerated(Exception): pass

# Takes in one chess.pgn.Game and generates fen strings of all unique positions
@cache
def Get_Fen_From_Game(Game:chess.pgn.Game) -> Iterator[str]:
    Board = Game.board()

    for move in Game.mainline_moves():
        Board.push(move)
        yield Board.fen()

# Takes in a PGN file and generates fen strings of all unique positions
@cache
def Get_Fen_Strings(file:str) -> Iterator[str]:
    fens  = set() 
    PGN   = open(file,'r')
    Games = []
   
    # Parse all games from PGN file
    CurrentGame = chess.pgn.read_game(PGN)
    while CurrentGame != None:
        Games.append(CurrentGame)
        CurrentGame = chess.pgn.read_game(PGN)

    # Iterate over all games and get all unique postions
    for Board in Games: 
        for fen in Get_Fen_From_Game(Board): fens.add(fen) 

    PGN.close()
    for fen in fens: yield fen

# Checks to see if we generate the same moves as the chess module
def All_Moves_Valid(Our_Moves:list, Validator_Moves:chess.LegalMoveGenerator) -> bool:
    Valid_UCI     = [chess.Move.uci(move) for move in Validator_Moves]
    Our_Moves_UCI = [Move_To_UCI(move)    for move in Our_Moves]
    
    # If the set of both moves are equal we have generate all correct moves
    return True if (set(Valid_UCI) == set(Our_Moves_UCI)) else False

# Takes in one fen and checks that we generate correct moves in this position
def Run_Test(fen:str) -> None:
    # Get both relevant board representations
    Validator_Board = chess.Board()
    Validator_Board.set_fen(fen)
    Our_Board = Fen_to_GameState(fen)
    
    # Get both relevant moves
    Our_Moves       = Generate_Moves(Our_Board, Our_Board.Side_To_Move)
    Validator_Moves = Validator_Board.legal_moves
    
    # Ensure that both the right number of moves has been generated and that all generated moves are legal
    No_Of_LegalMoves = Validator_Moves.count()

    if (len(Our_Moves) != No_Of_LegalMoves):
        print(f'{fen}\n')
        print('Error! Wrong number of moves generated.\n')
        raise WrongNumberOfMoves()

    if (not All_Moves_Valid(Our_Moves, Validator_Moves)):
        print(f'{fen}\n')
        print('Error! Illegal moves generated.\n')
        raise IllegalMovesGenerated()

if __name__ == "__main__":
    for fen in Get_Fen_Strings(TATA_STEEL_MASTERS_86TH):
        Run_Test(fen)

    print('All tests passed! :)\n')

    # List of fen strings that made the tests fail:
    # -----------------------------------------------

    # 2r2bk1/1pqn1p1p/p3bnp1/4p3/PPr1P3/2P1BP2/4N1PP/1RRNQBK1 b - - 0 22
    # - Fixed bug with new gamestate fen bitboard initialisations

    # 5r1k/pp4pp/8/5Pq1/P2NB3/3P4/1P4KN/3R4 w - - 5 29
    # - Fixed bug with sliding piece move generation, no moves will be generated
    # if the slider piece itself is part of occupancy

    # 3r2k1/p3qpp1/bpQ1n2p/4P3/5P2/4R1PP/PP3NB1/6K1 b - - 0 24
    # - Same bug as before but just making the fix in the move generator file

    # r1bqk2r/pppn1p2/5n1p/3p2p1/1b1P3B/2N1PN2/PP3PPP/R2QKB1R w KQkq - 0 9
    # - Get pinned pieces function is broken, error was in ray building,
    # ended up precomputing the ~2000 possible inputs for a space/time tradeoff, (at least its correct now)
   
    # r2q1rk1/ppp1bppp/2n2n2/2Pp3b/3P4/3BBN1P/PP1N1PP1/R2Q1RK1 b - - 4 11
    # - Pawns can jump over other pieces, fixed obstruction checks for pawns 

    # 2kr1b1r/1p3p1p/p7/5p2/8/4P1N1/PPn2PPP/R1B1K2R w KQ - 2 19
    # - King is still able to caste during check added better conditions to check for castles

    # rnb1k2r/ppppqppp/4pn2/8/1bPP4/5N2/PP1BPPPP/RN1QKB1R w KQkq - 4 5
    # - Issues with pinned piece detection where we now have the pin function return all bits
    # if their is no check instead of False

    # 2rR2k1/1b3p2/p3pn2/1p2n1p1/4P2p/1NN1KP1P/PP2B1P1/2R5 b - - 0 23
    # Silly bug in Is_Check function, accidentally put a 0 instead of a 1

    # 5r2/8/4Rb1k/4p2P/6B1/4P3/5PK1/8 b - - 6 46
    # - Can't have king capture a protected piece, related to the occupancy bug where no moves will be generated 
    # if slider piece itself is part of occupancy

    # 5rn1/5pk1/p5pB/8/1R6/P3N3/5r1P/3B2K1 b - - 0 29
    # - Some king moves haven't be generated, king moves should be ignored 
    # during filter generation

    # 4r2k/6p1/5q1p/8/2B5/KQ1NR3/5P2/r7 w - - 15 69
    # - If king is along line of attack from a rook or bishop
    # it must move off this line

    # r2qk2r/pp3p2/2pbbn1p/3p3R/3PP1p1/P1NB2P1/1P1NQPP1/R3K3 w Qkq - 1 18
    # - Queenside castle not generating correctly because of a typo

    # rn2k2r/pp2qppp/2n1p3/2PpPb2/8/N4N2/PPP1BPPP/R2Q1RK1 b kq - 1 10
    # - Queenside castle obstruction is missing a square

    # 4k3/2R5/4p2p/P4PpP/8/3bP3/2p2K2/8 b - - 0 62
    # - Upon reaching the last rank we must only show promotion moves cant move
    # to last rank without picking a promo

    # 1R6/5K2/2k5/5P2/8/8/1p6/5r2 w - - 2 78
    # - Pinmasks should reach the square infront of the king

    # 2r4k/5pbp/6p1/8/PQ3P2/1PNR4/2K3qP/7R w - - 5 32
    # - Knight is able to move of the pinned ray

    # 5r1k/pp4pp/8/5P2/P2NB2q/3P4/1P3K1N/3R4 w - - 3 28
    # - If piece is pinned and their is check combine the masks

    # r1b1r1k1/p2p1pbp/nppB1np1/2P2qN1/Q2P3P/8/PP2PNP1/R3KB1R w KQ - 2 16
    # - Misunderstood rule for disallowing queenside castles

    # 1r4k1/p6p/1n4pP/2b2pP1/2p1KP2/8/1P1R4/7R w - f6 0 36
    # - Haven't implemented en passant detection yet

    # r3r1k1/ppq2pp1/2pb1n1p/8/P1B5/2P2NP1/1P3PKP/R2Q1R2 b - - 0 18
    # - Faced an issue after refactoring Move generation code, the 
    # functions to check for starting ranks and promotion ranks were
    # returning memory addresses instead of actual functions

    # All positions from PGN file generate correct moves :)
