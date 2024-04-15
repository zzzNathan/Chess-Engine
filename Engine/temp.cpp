#include "MoveGen.hpp" 
#include <iostream>
using namespace std;
 
int main()
{
  string u = STARTING_FEN;
  Game Start(u);
  vector<Move> Moves = Generate_Moves(Start);

  for (Move m : Moves){
    if (m.UCI() == "g2g4"){
      Start.Make_Move(m);
      cout << "made move" << "\n";
      break;
    }
  }

  Moves = Generate_Moves(Start);
  for (auto m : Moves){
    if (m.UCI() == "h7h6"){
      Start.Make_Move(m);
      cout << "made move" << "\n";
      break;
    }
  }

  Moves = Generate_Moves(Start);
  for (auto m : Moves) cout << m.UCI() << "\n";
    
  Start.Show_Board();

  cout << Start.Get_Fen() << "\n";  
  cout << Start.Status.Side << "\n"; 
  
  cout << Moves.size() << "\n";  
  return 0;
}
