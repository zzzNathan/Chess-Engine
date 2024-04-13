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
#include "MoveGen.hpp"
#include <iostream>
using namespace std;

// The perft function aims to test the performance and correctness 
// of the move generation algorithm. The function will run a search
// up to a certain depth on the game's legal moves and print the 
// number of moves explored (nodes). In a perft function we only 
// consider the leaf nodes, that is only the moves that are found
// at the given depth to search.
//
// Good resource:
// https://chess.stackexchange.com/questions/22735/how-to-debug-move-generation-function
inline i64 Perft(Game game, int depth)
{
  vector<Move> Moves = Generate_Moves(game);
  if (depth == 1)
  { 
    cout << game.Get_Fen() << "\n";
    cout << Moves.size()   << "\n";
    for (Move m : Moves) cout << m.UCI() << "\n";
    
    return Moves.size();
  }

  else
  {   
    Game copy = game; // A copy of the game to restore state
    i64 nodes = 0;
          
    if (Moves.size() == 0) return 0; 
    for (Move move : Moves)
    { 
      game.Make_Move(move);
      nodes += Perft(game, depth - 1);
      game   = copy; // Restore state
    }
      
    return nodes;
  }
}

// WARNING: This function is not robust and potentially unsafe, because we
// don't verify correct input of fen strings. I am not resposible for any harm
// to the user's system when envoking the user manually envokes this function.
int main() 
{
  cout.tie(nullptr); // Faster output
  
  // depth 3 should be 62,379 nodes
  string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  //cout << "Enter the fen of the game you would like to perft test: \n";
  //getline(cin, fen);
                             
  int depth;
  cout << "Enter the depth you would like to search to: \n";
  cin  >> depth;
   
  Game game(fen);
  cout << "\nNodes: " << Perft(game, depth) << "\n\n";
 
  return 0;
}
