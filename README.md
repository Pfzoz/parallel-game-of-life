# Conway's Game of Life

Conway's Game of Life is an infinite game which path is dictated by its initial state.

More information about it here https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

## How to Build

You can run the make command to build using the Makefile, there are two main ways:

1. make
2. make PARALLEL=1

Setting the PARALLEL flag to 1 will enable openmp, highly increasing the game of life algorithm's speed.

To run just execute the main.x executable created.

## How to Use

The game starts in a Paused state, use keyboard SPACE to unpause/pause.
You can select/unselect the initial states by clicking on the screen.
You can press R to reset the game.
You can use key arrows UP and DOWN to increase and decrease, respectively, the delay at which (only) the algorithm is executed.
