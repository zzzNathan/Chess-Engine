# Simple Python chess engine

### :page_with_curl: Description:
This repository is a simple chess engine written in python, for the UK's 2025 Computer Science NEA component exam *(OCR)*.

### :clipboard: Dependencies:
- All dependencies can be installed with `pip install -r requirements.txt`
- Must have python installed (https://www.python.org/downloads/)
- Must have the python chess library installed to run the tests cases: `pip install chess` *(Ignore if you do not wish to run the test cases)*
- Must have numpy installed `pip install numpy`

### :white_check_mark: Features:
- [x] __Hyperbola quintessence__ for move generation (https://www.chessprogramming.org/Hyperbola_Quintessence)
- [ ] Alpha-Beta search -*Todo* 
- [ ] User Interface -*Todo*
- [x] Testing framework 
- [x] Piece square tables for evaluation
- [x] Get master level games in a pgn format for testing
- [x] Clean up the ClassUtilities file 
- [ ] Handle all types of draws -*Todo*

### Usage:
In order to run the testing scripts or any other desired script within this repository, ensure first that you have cloned this repository with:
```
git clone --depth 1 https://github.com/zzzNathan/Chess-Engine.git
```
Then run the following commands to run any script:
```
cd Chess-Engine
python -OO -m <Folder>.<Folder>...<Filename>
```
without a '.py' extension. Example: `python -OO -m Tests.Legal_Moves`
Enusure you are at the top of this directory before running any scripts.

### Project layout
<!---
For tree
tree -I '*.pyc|*.aux|*.log|*.toc|Assets|__*|NEA*'
-->
```
.
├── COPYING.txt
├── Engine
│   ├── Eval
│   │   ├── Evaluation.py
│   │   ├── OptimisedEval.py
│   │   └── PieceSquareTables.py
│   ├── MoveGen
│   │   ├── MoveGenUtils.py
│   │   └── MoveGenerator.py
│   ├── Search
│   │   └── Search.py
│   └── Utils
│       ├── BitMacros.py
│       ├── Build_Ray.py
│       ├── ClassUtilities.py
│       ├── Constants.py
│       └── Utilities.py
├── README.md
├── Tests
│   ├── Legal_Moves.py
│   └── PGN_Game_Files
│       ├── tatamast24.pgn
│       └── wchr23.pgn
├── pyrightconfig.json
└── requirements.txt
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
