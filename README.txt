Contributions
--------------------------------------------
Joey:
- Generate all possible moves using bitboard
- Implemented minimax/alpha-beta pruning
- Added transposition table/calculate initial opening moves
- Added iterative deepening

Ethan:
- Debugging bitboard algorithms
- Implemented and improved heuristics (experimenting with different options)
- Optimized performance on findBestMove using bitboards
- Tested possible optimizations with hashing


Generating Possible Moves
----------------------------------------------
We thought that it would be quicker to use the actual bitboard to 
generate all possible moves via C++ bit manipulations. We treated
the possible moves as a bitset, and individually found all 
possible movement positions in each direction, following this 
source we found online:

https://reversiworld.wordpress.com/2013/11/05/generating-moves-using-bitboard/


Transposition Tables
-----------------------------------------------
We added a transposition table to our AI program, so that it can
lookup board configurations instead of computing the same heurstic 
multiple times. The transposition table has a maximum size of 10000, 
and only stores the most recent board configurations. 

We also improved runtime by pre-computing some opening moves and
storing them in the transposition table before the start of the game, 
since we realized that the AI was taking the most time in the opening
of the game.


Minimax
-----------------------------------------------
We implemented the minimax algorithm with alpha-beta pruning. With pruning, 
we can search much farther than BetterPlayer can. 

We also used iterative deepening to search as far down the minimax tree as
possible while staying safely within the time bounds. This allowed the AI to
look many moves further ahead towards the endgame, where the minimax tree 
was easy to compute.

Heuristics
-----------------------------------------------
We came to the eventual conclusion that the best heuristic approach was one
that was simple, easy to compute, and therefore able to reach high move depths
in the tree search.  Our final heuristic returns +/- infinity for a winning or
losing board (no pieces remaining), and a very simple location-based heuristic
based off of coin count and corner weights otherwise.  Due to the bitset 
optimizations, this simple heuristic can effectively reach high depths, which
has the effect of weighting "mobility", "stability", and all other possible
heuristic measures.  As with a lot of AI, the simpler model is better.

https://kartikkukreja.wordpress.com/2013/03/30/heuristic-function-for-reversiothello/

Hashing
-----------------------------------------------
Hashing minimax_helper() was too costly to implement with alpha-beta pruning,
since this optimization means that minimax_helper() will return different values
for the same board but different values of alpha or beta.