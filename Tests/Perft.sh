#!/bin/bash  
# DO NOT RUN/MODIFY IF YOU DO NOT KNOW WHAT YOU ARE DOING

# This script is intended to check the validity and performance of the move generator
# through perft testing: https://www.chessprogramming.org/Perft. It will also aid
# in debugging the move generator. This script is intended to be ran on Linux/Unix 
# (as well as all other files), to run on Windows please reference:
# https://stackoverflow.com/questions/6413377/is-there-a-way-to-run-bash-scripts-on-windows

# Positions and results are from www.chessprogramming.org/Perft_Results and were
# cross-checked with Stockfish.

# We will take fen strings of the positions in our perft
# function then put the same fen strings into Stockfish
# and compare the moves outputted for both. To run the stockfish perft
# we assume that stockfish was git cloned into the same directory where
# this repository is located. Stockfish should be compiled normally,
# and we expect the executable to be found in Stockfish's /src directory
# 
# Stockfish: https://github.com/official-stockfish/Stockfish

# We assume that the user has GNU coreutils installed
# (https://en.wikipedia.org/wiki/GNU_Core_Utilities).
# This is true for most Linux distributions.

#(All moves are given in the UCI format 
# https://en.wikipedia.org/wiki/Universal_Chess_Interface)

# Our perft function gives output in the following format:
# _________________________________________________________
# ---------------------------------------------------------
# Enter the fen of the game you would like to perft test:
# Enter the depth you would like to search to:
# <FEN position 1>
# <Nodes at depth 1>
# <Move 1>
# <Move 2>
# ...
# <FEN position N>
# <Nodes at depth 1>
# <Move 1>
# ...
# Nodes: <Total number of nodes searched>
# <New line>
# _________________________________________________________
# 
# Stockfish's perft function gives output in the following
# format:
# _________________________________________________________
# ---------------------------------------------------------
# position fen <FEN position>
# go perft 1
# info string NNUE evaluation using <Name of NNUE>
# info string NNUE evaluation using <Name of NNUE>
# <Move 1>: 1
# <Move 2>: 1
# ...
# <Move N>: 1
# <New line>
# Nodes searched: <Total number of nodes searched>
# <New line>
# ________________________________________________________

# Create temporary files for both our perft and stockfish perft
OurMoves="OurMoves.txt"
SFMoves="SFMoves.txt"
FENlist="FENlist.txt"

touch $OurMoves
touch $SFMoves
touch $FENlist

# Gets the fen strings of all depth 1 positions from our perft function
# and puts them into FENlist.txt. Arguement 1 is the fen string, 
# arguement 2 is the depth.
Get_FENS() {
  cmd="$1\n$2"
  echo -e "$cmd" | ../Engine/Perft --fen > $FENlist

  # Remove first 2 lines of useless info
  Info=$(tail -n +3 $FENlist) 

  # Remove last line of useless info
  Info=$(echo "$Info" | head -n -1)

  echo -e "\n" >> $FENlist # Add a new line to indicate termination
  echo "$Info" > "$FENlist"
}

# Position 1
# -----------
# Perft of depth 2 on our perft function then write the 
# output to our temporary file
Get_FENS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "2"

# Left unfinished until OCR gives permission

# Cleanup the created files
rm $OurMoves
rm $SFMoves
rm $FENlist
