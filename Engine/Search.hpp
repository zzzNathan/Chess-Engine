/*
    Evergreen: A didactic C++ chess engine 
    Copyright (C) 2024  Jonathan Kasongo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <algorithm>
#include "MoveGen.hpp"
#include "Evaluate.hpp"

double INF = 1e9;

// For our searching algorithm we use a negamax algorithm
// NOTE: For negamax to work the evaluation must return 
// a score relative to the side being evaluated
double Search(Game game, int depth)
{
  if (depth == 0) return Evaluate(game); // Base case
  
  double best = -INF;
  Game copy   = game; // A copy of the game to restore state

  // Loop over all legal moves
  for (Move m : Generate_Moves(game))
  {
    game.Make_Move(m);
    best = max(best, -Search(depth - 1));
    game = copy; // Restore state
  }

  return best;
}
