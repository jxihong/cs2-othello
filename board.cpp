#include "board.h"

bitset<64> WEST = bitset<64>(0x7f7f7f7f7f7f7f7fULL);
bitset<64> EAST = bitset<64>(0xfefefefefefefefeULL);

/*
 * Shows valid positions to play in each direction: North, 
 * South, East, or West. Treats board configuration as a 
 * bitboard.
 */
bitset<64> N(bitset<64> x) {
    return x << 8;
}

bitset<64> S(bitset<64> x) {
    return x >> 8;
}

// Have to use strings because 64 bits is larger than long integers.
bitset<64> W(bitset<64> x) {
    return (x & WEST) << 1;
}

bitset<64> E(bitset<64> x) {
    return (x & EAST) >> 1;
}

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);

    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}

 
/*
 * Returns true if the game is finished; false otherwise. The game is finished 
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == NULL) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A NULL move means pass.
    if (m == NULL) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Generates all possible moves for a specific side. To avoid
 * return an entire vector, the vector to be filled is passed
 * in by reference as an argument.
 */
void Board::getPossibleMoves(Side side, vector<Move *>& possibleMoves) {
    possibleMoves.clear(); // Makes sure vector is empty
   
    bitset<64> own;
    bitset<64> opp;
    if (side == BLACK) {
	own = black;
	opp = taken & (~black);
    }
    else {
	own = taken & (~black);
	opp = black;
    }

    bitset<64> moves(0x0);
    
    // Gets all moves possible by playing above an enemy piece
    bitset<64> possible = N(own) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= N(possible) & opp;
    }
    moves |= N(possible) & (~taken);
    
    // Gets all moves possible by playing below an enemy piece
    possible = S(own) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= S(possible) & opp;
    }
    moves |= S(possible) & (~taken);
    
    // Gets all moves possible by playing right of an enemy piece
    possible = E(own) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= E(possible) & opp;
    }
    moves |= E(possible) & (~taken);
    
    // Gets all moves possible by playing left of an enemy piece
    possible = W(own) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= W(possible) & opp;
    }
    moves |= W(possible) & (~taken);
    
    // Gets all moves possible by playing diagonally right above 
    // an enemy piece
    possible = N(E(own)) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= N(E(possible)) & opp;
    }

    moves |= N(E(possible)) & (~taken);
    
    // Gets all moves possible by playing diagonally left above 
    // an enemy piece
    possible = N(W(own)) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= N(W(possible)) & opp;
    }
    moves |= N(W(possible)) & (~taken);
    
    // Gets all moves possible by playing diagonally right below 
    // an enemy piece
    possible = S(E(own)) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= S(E(possible)) & opp;
    }
    moves |= S(E(possible)) & (~taken); 

    // Gets all moves possible by playing diagonally left below 
    // an enemy piece
    possible = S(W(own)) & opp;
    for (int i = 0; i <= 4; ++i) {
	possible |= S(W(possible)) & opp;
    }
    moves |= S(W(possible)) & (~taken);
    
    /* FOR DEBUGGING
    for (unsigned int i = 0; i < taken.size(); ++i) {
	if (taken.test(i)) {
	    if (black.test(i)) {
		cerr << "B";
	    }
	    else {
		cerr << "W";
	    }
	}
	else {
	    cerr << "0";
	}
	if (i % 8 == 7) {
	    cerr << endl;
	}
    }
    cerr << endl;

    for (unsigned int i = 0; i < moves.size(); ++i) {
	if (moves.test(i)) {
	    cerr << "1";
	}
	else {
	    cerr << "0";
	}
	if (i % 8 == 7) {
	    cerr << endl;
	}
    }
    cerr << endl;
    */
    
    // Populates the vector of possible moves with the moves
    // that were obtained through bit manipulations.
    for (unsigned int i = 0; i < moves.size(); ++i) {
	if (moves.test(i)) {
	    int x = i % 8;
	    int y = i / 8;
	    Move *next = new Move(x, y);
	    possibleMoves.push_back(next);
	}
    }    
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}
