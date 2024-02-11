#/*******************************************\
#   M O V E  G E N E R A T I O N  T E S T S        
#   - - - -  - - - - - - - - - -  - - - - -      
#\*******************************************/

# The following code aims to test if the file
# "MoveGenerator.py" generates legal, playable moves

# https://chess.stackexchange.com/questions/40089/uniformly-generate-random-chess-positions

from Engine.MoveGenerator import *
import chess
import chess.pgn
import os

LOCAL_DIR = os.path.dirname(__file__)
TATA_STEEL_MASTERS_86TH = os.path.join(LOCAL_DIR ,r'PGN_Game_Files/tatamast24.pgn')

# Takes in one chess.pgn.Game and generates fen strings of all
# uniqe positions
def Get_Fen_From_Game(Game:chess.pgn.Game) -> set:
    
    fens = set()
    Board = Game.board()
    for move in Game.mainline_moves():

        Board.push(move)
        fens.add( Board.fen() )

    return fens

# Takes in a PGN file and generates fen strings of all 
# unique positions
def Get_Fen_Strings(file:str) -> list:
    
    fens  = set()
    PGN   = open(file,'r')
    Games = []
    
    # Parse all games from PGN file
    while 1:
        CurrentGame = chess.pgn.read_game(PGN)
        if CurrentGame == None: break
        else: Games.append(CurrentGame)

    # Iterate over all games and get all unique postions
    for Board in Games:
       
        fens.update( Get_Fen_From_Game(Board) )

    return list( fens )

# Takes in a list of the moves that we have generated and 
# checks to see if they are also in the moves that our validator 
# has generated
def All_Moves_Valid(Our_Moves:list, Validator_Moves:chess.LegalMoveGenerator) -> bool:
    
    # Check if all the move we have generator exist in the 
    # validator's move list
    for move in Our_Moves:
        
        # Convert our move object into a PyChess move object
        PyChessMove = chess.Move.from_uci(Move_To_UCI(move))
        if PyChessMove not in Validator_Moves: return False
    
    # If all moves are found inside the validator return true
    return True

# Takes in a list of unique fen strings and checks that 
# our move generator generates legal and correct moves each time
def Run_Tests(fens:list) -> None:
    
    for fen in fens:
        
        # Get both relevant board representations
        Validator_Board = chess.Board()
        Validator_Board.set_fen(fen)
        Our_Board = Fen_to_GameState(fen)
        
        # Get both relevant moves
        Our_Moves       = Generate_Moves(Our_Board, Our_Board.Side_To_Move)
        Validator_Moves = Validator_Board.legal_moves
        
        # Ensure that both the right number of moves has been generated and that all generated moves are legal
        if len(Our_Moves) != Validator_Moves.count(): print(fen)
        assert len(Our_Moves) == Validator_Moves.count(), 'Error! Wrong number of moves generated.'

        assert All_Moves_Valid(Our_Moves, Validator_Moves), 'Error! Illegal moves generated.'

    print('All Tests Passed! :)')
        
if __name__ == "__main__":
    #Run_Tests( Get_Fen_Strings(TATA_STEEL_MASTERS_86TH) )
    
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
    # - Get pinned pieces function is broken
   
    # For debugging:
    bugfen = r'r1bqk2r/pppn1p2/5n1p/3p2p1/1b1P3B/2N1PN2/PP3PPP/R2QKB1R w KQkq - 0 9'
    
    valid = chess.Board()
    valid.set_fen( bugfen )
    valids = [chess.Move.uci(move) for move in valid.legal_moves] 
    
    bd = Fen_to_GameState( bugfen )
    for move in Generate_Moves( bd, bd.Side_To_Move ):

        if Move_To_UCI(move) not in valids: print( Move_To_UCI(move) )
        #print( Move_To_UCI(move) )
    
    print('-'*20)
    print( Show_Board(bd) ) 
    print( bd.Pins, Get_Pinned_Pieces('w',bd) )
