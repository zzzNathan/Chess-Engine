#include "MoveGen.hpp"

int main()
{
  string u = STARTING_FEN;
  Game Start(u);
  vector<Move> Moves = Generate_Moves(Start);
 

  Start.Show_Board();

  for (const Move& move : Moves)
    cout << move.UCI() << "\n"; 
  
  return 0;
}
