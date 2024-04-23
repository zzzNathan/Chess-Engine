# Simple makefile for compiling the move generator

# These are the files that need to be compiled to get the move generator
MOVE_SOURCES = Utility/Utils.cpp Moves.cpp MoveGen.cpp

# The main program
Evergreen:
	g++ ${MOVE_SOURCES} temp.cpp -o Evergreen 

# The perft file
perft:
	g++ ${MOVE_SOURCES} Perft.cpp -o Perft -Os
