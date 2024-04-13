#include "MoveGen.hpp" 
#include <iostream>
using namespace std;
 
int main()
{
  string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  //fen = "rnNq1k1r/pp2bppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R b KQ - 0 8";
  Game Start(fen);
  vector<Move> Moves = Generate_Moves(Start);
  //Game cp = Start;
    
  Start.Show_Board();
  //for (auto m : Moves) cout << m.UCI() << "\n";
  /*
  cout << Start.Status.Pins.size() << "\n";
  for (auto p : Start.Status.Pins){
    Show_Bitboard(p.first);
    cout << "\n" << "--------" << "\n";
    Show_Bitboard(p.second);
    cout << "\n" << "--------)" << "\n";
  }
  */ 
  Start.Show_Board();
  cout << Start.Get_Fen() << "\n";  
 
  // cp.Show_Board();
  
  cout << Moves.size() << "\n";  
  return 0;
}
