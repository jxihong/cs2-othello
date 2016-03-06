Caltech CS2 Assignment 9: Othello

See [assignment9_part1.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part1.html) and [assignment9_part2.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part2.html)

Test: I can commit!

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
