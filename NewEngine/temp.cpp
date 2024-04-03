#include "MoveGen.h" 
#include <iostream>
using namespace std;

int main(){
  string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Game Start(fen);
  vector<Move> Moves = Generate_Moves(Start);
            
  for (Move m : Moves){
    Show_Bitboard(m.From);
    cout << "\n";
    Show_Bitboard(m.To);
    cout << m.Piece << " " << (m.Capture ? "take" : "none") << (m.En_Passant ? "french" : "english") << "\n";
    cout << "\n\n";
  }  
  cout << "\n";
 
  Start.Show_Board();
  Show_Bitboard(Start.Status.White_Check);

  cout << Moves.size() << "\n";  

  return 0  ;
}
