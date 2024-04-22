#include "NewMoveGen.hpp"

int main()
{
  string u = STARTING_FEN;
  Game Start(u);
  vector<Move> Moves = Generate_Moves(Start);
   
  cout << "hii" << "\n";
  return 0;
}
