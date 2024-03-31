#/*******************************************\
#                S E A R C H 
#                - - - - - - 
#\*******************************************/
from Engine.Eval.Evaluation import *

# Reference:
# ----------
# - https://youtu.be/I-hh51ncgDI

# White will try to maximise evaluation 
# Black will try to minimise evaluation

# We will iterate through all moves then search through all
# of them after which we will simply choose to play the move
# with the best evalation.

@cache
def Minimax(Game:GameState, depth:int, Maximise:bool) -> float:
    if depth == 0: return Evaluate(Game)
    
    col = 'w' if (Maximise) else 'b'
    if Maximise:
        bestEval = -INF

        for move in Generate_Moves(Game, col):
            Game.Make_Move(move)
            Eval = Minimax(Game, depth - 1, not Maximise)
            bestEval = max(Eval, bestEval)
            Game.Unmake_Move()

        return bestEval

    else:
        minEval = INF
        for move in Generate_Moves(Game, col):
            Game.Make_Move(move)
            Eval = Minimax(Game, depth - 1, not Maximise)
            minEval = min(Eval, minEval)
            Game.Unmake_Move()

        return minEval

def FindMove(Game:GameState):
    Maximising = True if (Game.Side_To_Move == 'w') else False 
    bestmove, besteval = None, None
    Depth = 7
    for move in Generate_Moves(Game, Game.Side_To_Move):
        if (bestmove == None):
            bestmove = move
            besteval = Minimax(Game, Depth, Maximising)

        else:
            Eval = Minimax(Game, Depth, Maximising)
            print(Move_To_UCI(move), Eval)
            if (Eval > besteval):
                bestmove = move
                besteval = Eval

    return Move_To_UCI(bestmove)

print(FindMove(STARTING_GAME))
print(Show_Board(STARTING_GAME))
