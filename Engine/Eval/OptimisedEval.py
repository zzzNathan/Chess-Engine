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
# differentiation. We use here an approximation on the partial derivative using the 
# formal limit definition, namely we pick some arbitrarily small value of h and 
# compute f(a[1], a[2], ... ,a[i] + h, ... ,a[n]) / h, we then compute
# f(a[1], a[2], ... ,a[i] - h, ... ,a[n]) / -h, and pick the direction with the minimal
# derivative. By direction we consider +h to be "right" and -h to be "left"
# after we find the minimal direction we adjust our damper towards this direction.
# If both directions produce positive gradients then we will have reached a local 
# minimum and we will stop here. A more detailed description will be given in my NEA.

# TODO - implement learning rates

MAX_ITER = 1_000
MIN_DAMPER = 0
MAX_DAMPER = 1_000
# Start off by picking some random values for our dampers
DAMPING = {
    'Material'      : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'Mobility'      : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'Connectivity'  : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'DoubledPawns'  : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'OpenRookFiles' : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'PassedPawns'   : np.random.uniform(MIN_DAMPER, MAX_DAMPER),
    'Space'         : np.random.uniform(MIN_DAMPER, MAX_DAMPER)
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
    Fens = Get_Fen_Strings(r'Tests/PGN_Game_Files/kasparov_radjabov_2003.pgn') 
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

# Computes an estimate for the partial derivative based on description at the top of the file
def Partial_Deriv(damperName:str, CurrDamper:float) -> tuple[float|None, float|None]:
    h = 0.5 # The arbitrarily small value used to estimate the limit
    
    CurrentError = Error()
    
    # Moving forwards
    damper_Plus_h = CurrDamper + h
    DAMPING[damperName] = damper_Plus_h
    Error_Plus_h  = Error()
    
    # Moving backwards
    damper_Minus_h = CurrDamper - h
    DAMPING[damperName] = damper_Minus_h
    Error_Minus_h = Error()
    
    # Break out if any of the error calculations fail
    if (CurrentError == None or Error_Plus_h == None or Error_Minus_h == None):
        return None, None

    Forward_Deriv  = (Error_Plus_h - CurrentError) / h
    Backward_Deriv = (Error_Minus_h - CurrentError) / -h

    return Forward_Deriv, Backward_Deriv

# Tunes evaluations based on description given at the top of the file
def TuneEval():
    # Iterate over all dampers and find value for which they are minimal
    for damperName in DAMPING: 
        h = 0.5 # The arbitrarily small value used to estimate the limit
        damper = DAMPING[damperName] 
        Forward_Derivative, Backward_Derivative = Partial_Deriv(damperName, damper)

        # If the derivatives are 'None' then calculation of error function has failed
        if (Forward_Derivative == None or Backward_Derivative == None):
            print('Error function failed!')
            return 

        # If both derivatives show that the function is increasing, this must be a 
        # local minimum
        if (Forward_Derivative > 0 and Backward_Derivative > 0): continue
        
        count = 0 # Keep a count of iterations so that we don't have an infinite loop
        while ((Forward_Derivative < 0 and Backward_Derivative < 0) and (count < MAX_ITER)):
            # The more negative the partial derivative is, the more we should increase our 
            # learning rate

            # Adjust damper to move in the direction where the derivative is minimised
            if (Forward_Derivative < Backward_Derivative): damper += h * LearnRate
            else: damper -= h * LearnRate
            
            # Re-compute partial derivative
            Forward_Derivative, Backward_Derivative = Partial_Deriv(damperName, damper)    

            # If the derivatives are 'None' then calculation of error function has failed
            if (Forward_Derivative == None or Backward_Derivative == None):
                print('Error function failed!')
                return 
            
            count += 1
