# MineSweeper

Minesweeper made using the C language!  
We took inspiration from [Simon Tatham's Portable Puzzle Collection](https://www.chiark.greenend.org.uk/~sgtatham/puzzles/) Minesweeper implementation  

## How to compile and run (linux)
- You'll need CMake installed on your distro.
- On a terminal, run the following commands: 
```
git clone https://github.com/Trabalhos-PUC-PR/ProgImperativa-MineSweeper.git
cd ProgImperativa-MineSweeper
cmake CMakeLists.txt
cmake --build .
```

- To open the game via CLI, run the following command where the compiled minesweeper is located:
```
./MineSweeper
```
- Or alternatively, double click the executable on your favorite file browser

## Dependencies:
- GTK+3

## Features:
- 3 difficulties to chose from
- **Absolutely random** bomb placement
- Customize your field with your own preferences
- Your first guess will **never** be a bomb!
- Counters that update in **real time** to keep track of how many bombs and squares are left!
