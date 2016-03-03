Caltech CS2 Assignment 9: Othello

See [assignment9_part1.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part1.html) and [assignment9_part2.html](http://htmlpreview.github.io/?https://github.com/caltechcs2/othello/blob/master/assignment9_part2.html)

Test: I can commit!

Part 1: Generating Possible Moves
----------------------------------------------
We thought that it would be quicker to use the actual bitboard to 
generate all possible moves via C++ bit manipulations. We treated
the possible moves as a bitset, and individually found all 
possible movement positions in each direction, following this 
source we found online:

https://reversiworld.wordpress.com/2013/11/05/generating-moves-using-bitboard/
