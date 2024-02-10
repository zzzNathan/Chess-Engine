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

### Usage:
In order to run the testing scripts or any other desired script within this repository, ensure first that you have cloned this repository with:
```
git clone https://github.com/zzzNathan/Chess-Engine.git
```
Then ensure that you are at the root of this repository and run:
```
python -m <Folder>.<Filename>
```
without a '.py' extension. Example: `python -m Tests.Legal_Moves`

### Project layout
```
.
├── Engine
│   ├── BitMacros.py
│   ├── ClassUtilities.py
│   ├── ConstantsAndTables.py
│   ├── Evaluation.py
│   ├── MoveGenerator.py
│   ├── PieceSquareTables.py
│   ├── Search.py
│   ├── Utilities.py
│   ├── __init__.py
│
├── README.md
│
├── Tests
│   ├── Legal_Moves.py
│   ├──_init__.py
└── __init__.py
```

May re-write in a faster language like c++ in the future.

//
__*Currently a work in progress.*__
