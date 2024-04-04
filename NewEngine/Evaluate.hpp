/*
    Shallow-Thought: A didactic C++ chess engine 
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
#include "Game.hpp"

// A function to get the 'phase' of a game
// In chess there are 3 main phases of the game:
// ---------------------------------------------
// - Opening
//
// - Middlegame
//
// - Endgame
// ---------------------------------------------
// A high phase denotes that we are nearing the endgame
// whilst a low phase denotes that we are still in the 
// opening/middlegame phase. In this engine we use 
// Fruit's definition of phase. https://www.fruitchess.com/
double Get_Game_Phase(Game game)
{

}

double Evaluate(Game game)
{
  return 0.0;
}
