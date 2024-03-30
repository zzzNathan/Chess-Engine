#/*******************************************\
#            E V A L U A T I O N S  
#            - - - - - - - - - - - 
#\*******************************************/
from Engine.Eval.Evaluation import *
from Tests.Legal_Moves      import Get_Fen_Strings
# This script aim to optimise our evaluation function,
# the main idea is that some evaluation features are generally more important 
# than others and this contributes significantly to the performance of 
# of our engine.
#
# To apply this idea we create some scalars which I call 'dampers',
# these values will either increase or decrease the different components of
# evaluation. For instance if the 'Material' damper is 0.25 then the scores
# from material will be decreased by 75% and will now have significantly less
# weighting in the entire evaluaton.
#
# Each of the dampers will have some 'optimal' value, in which the evaluation 
# performs the best. To quantify the performance of our evaluation we assume 
# that Stockfish's evaluation is perfect and always gives an 'objectively' correct
# evaluation. We use the average squared error of Stockfish's evaluation minus
# our evaluation, and hence we try to minimise this average squared error.
#
# One method to minimise our average squared error is by using partial 
# differentiation

MIN_DAMPER = 0
MAX_DAMPER = 1000
DAMPING = {
    'Material'      : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'Mobility'      : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'Connectivity'  : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'DoubledPawns'  : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'OpenRookFiles' : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'PassedPawns'   : np.random.randint(MIN_DAMPER, MAX_DAMPER),
    'Space'         : np.random.randint(MIN_DAMPER, MAX_DAMPER)
    }

# Calculates the average squared error
def Error(Game:GameState) -> float:
    Fens = Get_Fen_Strings(r'Tests/PGN_Game_Files/wchr23.pgn') 
    
    for fen in Fens:
        


# Evaluates the current position
def Evaluate(Game:GameState) -> float:
    Score = 0

    # Check for a stalemate
    if (Generate_Moves(Game, Game.Side_To_Move) == []) and (Is_Check(Game.Side_To_Move, Game) == AllBits): 
        return 0
    # Checks for a 3-fold repition
    if Game.Draw: return 0

    # Check if white of black has checkmate
    if Is_Check('w',Game)!=AllBits and Generate_White_King_Moves(Game.WhiteKing,Game) == []: return -INF
    if Is_Check('b',Game)!=AllBits and Generate_Black_King_Moves(Game.BlackKing,Game) == []: return INF

    Score += (Material(Game, 'w')    - Material(Game, 'b'))    
    Score += (Mobility(Game, 'w')    - Mobility(Game, 'b'))    
    Score += Connectivity(Game, 'w') - Connectivity(Game, 'b') 
    Score += DoubledPawns(Game, 'w') - DoubledPawns(Game, 'b') 
    Score += OpenRookFiles(Game,'w') - OpenRookFiles(Game,'b') 
    Score += PassedPawns(Game ,'w')  - PassedPawns(Game, 'b')  
    Score += Space(Game, 'w')        - Space(Game, 'b')        
    return Score
