#include "MoveGen.hpp"

int main()
{
  string u = STARTING_FEN;
  Game Start(u);
  vector<Move> Moves = Generate_Moves(Start);

  for (const Move& move : Moves)
    cout << move.UCI() << "\n"; 
  
  return 0;
}
