# This script aims to compare the divide function of my
# engine and the divide function of stockfish.

# Firstly take in our moves
Our_Moves = []
for _ in range(int(input())): Our_Moves.append(input())

# Then take in the moves from stockfish
Stockfish_Moves = []
for _ in range(int(input())): Stockfish_Moves.append(input())

Our_Moves.sort()
Stockfish_Moves.sort()

if Our_Moves == Stockfish_Moves: print("Correct moves generated")
else: 
    print("Incorrect moves generated")
    Our_Moves       = set(Our_Moves)
    Stockfish_Moves = set(Stockfish_Moves)

    if len(Our_Moves) > len(Stockfish_Moves):
        print("The following moves are illegal:")

        for move in (Our_Moves - Stockfish_Moves): print(move)

    else:
        print("The following moves are missing:")

        for move in (Stockfish_Moves - Our_Moves): print(move)
