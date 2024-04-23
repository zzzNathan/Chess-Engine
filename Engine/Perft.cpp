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

using namespace std;

// The perft function aims to test the performance and correctness 
// of the move generation algorithm. More specifically this function 
// is a divide function (a variation on perft). The function will run
// a search up to a certain depth on the game's legal moves and print
// the number of moves explored (nodes). In a perft function we only 
// consider the leaf nodes, that is only the moves that are found
// at the given depth to search.
//
// Good resource:
// - https://chess.stackexchange.com/questions/22735/how-to-debug-move-generation-function
template<bool Root>
inline unsigned long long Perft(Game& game, int depth)
{
  vector<Move> Moves = Generate_Moves(game);
  
  if (depth == 1)
  {
    if (Root)
    {
      for (const Move& move : Moves) 
        cout << move.UCI() << ": 1" << "\n";
    }

    return Moves.size();
  }

  unsigned long long nodes = 0;
  unsigned long long delta = 0;
        
  for (const Move& move : Moves)
  { 
    game.Make_Move(move);
    delta = Perft<false>(game, depth - 1);
    
    if (Root) cout << move.UCI() << ": " << delta << "\n";
    
    nodes += delta;
    game.Unmake_Move(move);
  }
    
  return nodes;
}

// WARNING: This function is not robust and potentially unsafe, because we
// don't verify correct input of fen strings. I am not resposible for any harm
// to the user's system when envoking the user manually envokes this function.
int main() 
{
  cout.tie(nullptr); cin.tie(nullptr); // Fast I/O
  
  //string fen;
  string fen = STARTING_FEN;
  //fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

  //cout << "Enter the fen of the game you would like to perft test: \n";
  //getline(cin, fen);
 
  int depth;
  cout << "Enter the depth you would like to search to: \n";
  cin  >> depth;
   
  Game game(fen);
  unsigned long long nodes = Perft<true>(game, depth);

  cout << "Nodes: " << nodes << "\n\n";
  game.Show_Board();
 
  return 0; 
}
