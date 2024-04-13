#!/bin/bash  
# This script is intended to check the validity and performance of the move generator
# through perft testing: https://www.chessprogramming.org/Perft. It will also aid
# in debugging the move generator. This script is intended to be ran on Linux/Unix 
# (as well as all other files), to run on windows please reference:
#
# https://stackoverflow.com/questions/6413377/is-there-a-way-to-run-bash-scripts-on-windows

# Positions and results are from www.chessprogramming.org/Perft_Results and were
# cross-checked with Stockfish.

Our_file="perft_temp.txt"
Stockfish_file="perft_stockfish.txt"

touch $Temp_File
touch $Stockfish_file

# Position 1
echo "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" | /Engine/.Perft

# Cleanup the created file
rm $Temp_File
