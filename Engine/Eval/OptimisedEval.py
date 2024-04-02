#/*******************************************\
#            E V A L U A T I O N S  
#            - - - - - - - - - - - 
#\*******************************************/
from math import e
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
# A more detailed description will be given in my NEA.

MAX_ITER = 2_000
MIN_DAMPER = 0
MAX_DAMPER = 1_000
# Values found by the evaluation tuner 
DAMPING = {
    'Material'      : 95,
    'Mobility'      : 1000,
    'Connectivity'  : 10,
    'DoubledPawns'  : 500,
    'OpenRookFiles' : 500,
    'PassedPawns'   : 505,
    'Space'         : 90 
    }

# Evaluates the current position
@cache
def Evaluate(Game:GameState) -> float:
    Score = 0
    Moves = list(Generate_Moves(Game, Game.Side_To_Move))

    # Check for a stalemate
    if (Moves == []) and (Is_Check(Game.Side_To_Move, Game) == AllBits): 
        return 0
    # Checks for a 3-fold repition
    if Game.Draw: return 0

    # Check if white of black has checkmate
    if Is_Check('w',Game)!=AllBits and Generate_White_King_Moves(Game.WhiteKing,Game) == []: return -INF
    if Is_Check('b',Game)!=AllBits and Generate_Black_King_Moves(Game.BlackKing,Game) == []: return INF

    Score += (Material(Game, 'w')    - Material(Game, 'b'))      * DAMPING['Material'] 
    Score += (Mobility(Game, 'w')    - Mobility(Game, 'b'))      * DAMPING['Mobility'] 
    Score += (Connectivity(Game, 'w') - Connectivity(Game, 'b')) * DAMPING['Connectivity'] 
    Score += (DoubledPawns(Game, 'w') - DoubledPawns(Game, 'b')) * DAMPING['DoubledPawns'] 
    Score += (OpenRookFiles(Game,'w') - OpenRookFiles(Game,'b')) * DAMPING['OpenRookFiles'] 
    Score += (PassedPawns(Game ,'w')  - PassedPawns(Game, 'b'))  * DAMPING['PassedPawns'] 
    Score += (Space(Game, 'w')        - Space(Game, 'b'))        * DAMPING['Space'] 
    return Score

# Calculates the mean squared error over evaluations from many positions
@cache
def Error(fast:bool=False) -> float|None:
    Fens = [STARTING_FEN, TRICKY_POS_FEN, RANDOM_FEN]
    N = 0 # Number of positions to test
    
    error = 0
    for fen in Fens:
        N += 1
        OurEval = Evaluate(Fen_to_GameState(fen))
        SF_Eval = Get_SF_Eval(fen, fast)

        # Check that stockfish evaluation has worked
        if (SF_Eval == None): return

        error += (SF_Eval - OurEval) ** 2

    error /= N
    return error

# Code to tune evaluations to find their most optimal dampings
def TuneEval():
    # Iterate over all dampers and find value for which they are minimal
    for damperName in DAMPING: 
        damper = DAMPING[damperName] 
        
        bestdamp  = damper
        lowestErr = Error(True)

        for i in range(0,1001, 5):
            DAMPING[damperName] = i
            currErr = Error(True)
            
            # If this error is less than our previous then we should take 
            # this damper
            if (currErr < lowestErr):
                bestdamp = i
                lowestErr = currErr

        DAMPING[damperName] = bestdamp

    for k,v in DAMPING.items():
        print(k, v)

if __name__ == "__main__":
    TuneEval()
