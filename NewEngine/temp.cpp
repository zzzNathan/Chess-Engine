#include "MoveGen.hpp" 
#include <iostream>
using namespace std;

int main(){
  string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Game Start(fen);
  vector<Move> Moves = Generate_Moves(Start);
  
  Start.Show_Board();

  cout << Moves.size() << "\n";  

  return 0  ;
}
