# Simple Python chess engine

### :page_with_curl: Description:
This repository is a simple chess engine written in python, for the UK's 2025 Computer Science NEA component exam *(OCR)*.

### :clipboard: Dependencies:
- Must have python installed (https://www.python.org/downloads/)
- Must have the python chess library installed to run the tests cases: `pip install chess` *(Ignore if you do not wish to run the test cases)*
- Must have numpy installed `pip install numpy`

### :white_check_mark: Features:
- [x] __Hyperbola quintessence__ for move generation (https://www.chessprogramming.org/Hyperbola_Quintessence)
- [ ] Alpha-Beta search -*Todo* 
- [ ] User Interface -*Todo*
- [ ] Testing framework -*Todo*
- [x] Piece square tables for evaluation
- [x] Get master level games in a pgn format for testing
- [ ] Clean up the ClassUtilities file -*Todo*

### Usage:
In order to run the testing scripts or any other desired script within this repository, ensure first that you have cloned this repository with:
```
git clone --depth 1 https://github.com/zzzNathan/Chess-Engine.git
```
Then run the following commands to run any script:
```
cd Chess-Engine
python -m <Folder>.<Filename>
```
without a '.py' extension. Example: `python -m Tests.Legal_Moves`

### Project layout
```
.
├── Engine
│   ├── BitMacros.py
│   ├── Build_Ray.py
│   ├── ClassUtilities.py
│   ├── ConstantsAndTables.py
│   ├── Evaluation.py
│   ├── MoveGenerator.py
│   ├── PieceSquareTables.py
│   ├── Search.py
│   ├── Utilities.py
│   └── __init__.py 
│
├── README.md
│
└── Tests
    ├── Legal_Moves.py
    ├── PGN_Game_Files
    │   └── tatamast24.pgn
    └── __init__.py
```
---
__*Note*__: For future development, ensure to configure your python
language server protocol to make the root the top of the
Chess-Engine directory. The code below is how you would
configure Pyright language server protocol: 

```
"executionEnvironments":[
    {"root":"<PATH_TO_DIRECTORY>/Chess-Engine"}
]
```
This code should be in a file called `pyrightconfig.json` at
the top of the Chess-Engine directory.

(May re-write in a faster language like c++ in the future.)

__*// Currently a work in progress.*__
