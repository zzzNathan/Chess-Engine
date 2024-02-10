#/*******************************************\
#   M O V E  G E N E R A T I O N  T E S T S        
#   - - - -  - - - - - - - - - -  - - - - -      
#\*******************************************/

# The following code aims to test if the file
# "MoveGenerator.py" generates legal, playable moves

# Add this dir to engine dir ?

from Engine.MoveGenerator import *
import chess

# A list of fen strings to be used as testing postions
POSITIONS = [STARTING_FEN]

def Run_Tests() -> None:
    for fen in POSITIONS:

        Game = Fen_to_GameState( fen )
        Validator = chess.Board( fen )

        # Iterate over all moves that we generate and assert that they exist in the 
        # valdidator's legal moves list also
        for move in Generate_Moves(Game, Game.Side_To_Move):
            
            # Convert our move object into a pychess move object
            PychessMove = chess.Move.from_uci(Move_To_UCI(move)) 
            assert PychessMove in Validator.legal_moves, 'Error: Illegal Move!'

if __name__ == "__main__":
    Run_Tests()
