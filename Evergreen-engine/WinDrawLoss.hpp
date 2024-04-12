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
#ifndef WIN_DRAW_LOSS_HPP
#define WIN_DRAW_LOSS_HPP
// | Win | Draw | Loss | Verification functions
// ---------------------------------------------
// This file is to be included at the bottom of "MoveGen.hpp", this is because
// it depends on functions and classes from both "MoveGen.hpp" and "Game.hpp",
// however these functions don't have anything to do with move generation and 
// therefore don't belong in the "MoveGen.hpp" file
//
// Function to check whether any side has checkmated the other
uint8_t Game::Check_Win()
{
  // If there currently is a check and we have no legal moves we have check mate
  if (Status.Side == WHITE)
  {
    if (Status.White_Check != NONE && Generate_Moves(*this).size() == 0) return -1;
  }
  else 
  {
    if (Status.Black_Check != NONE && Generate_Moves(*this).size() == 0) return -1;
  }
  return 2; // Otherwise game is still in play
}

// Function to check whether the game is a draw
uint8_t Game::Check_Draw()
{
  // 50 move-rule draws
  if (Status.Ply >= MAX_PLY) return 0;

  return 2; // Otherwise game is still in play
}
#endif
