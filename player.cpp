#include "player.h"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) : _side(side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    
    /* 
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
    _board = new Board();
    _opponentSide = (_side == BLACK) ? (WHITE) : (BLACK);
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    delete _board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    /* 
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */ 
    if (opponentsMove) {
	_board->doMove(opponentsMove, _opponentSide);
    }
    
    Move *m = (testingMinimax) ? 
	(this->findMinimaxMove(2)) : (this->findMinimaxMove(5));
    _board->doMove(m, _side);

    return m;
}

/*
 * Returns the first available move that the AI finds.
 */
Move *Player::findFirstMove() {
    vector<Move *> moves;
    _board->getPossibleMoves(_side, moves);
    
    // Returns the first move in the list
    if (!moves.empty()) {
	return moves.front();
    }
    
    // No valid moves
    return NULL;
}

/*
 * Uses the minimax algorithm to look for the best move
 */
Move *Player::findMinimaxMove(int depth) {
    Board *copyboard = _board->copy();
   
    // Calls helper function that returns best (score, move) pair 
    pair<int, Move *> best = this->minimaxHelper(depth, copyboard, _side);
    delete copyboard;
    
    return best.second;
}

/*
 * Heuristic that evaluates the score of a given board
 * configuration.
 */ 
int Player::evaluate(Board *b) {
    if (testingMinimax) {
	return b->count(_side) - b->count(_opponentSide);
    }
    else {
	/* TODO: Update the heuristic with a better one that takes into
	 * account position of pieces, frontier, stability, etc.
	 */
	return b->count(_side) - b->count(_opponentSide); 
    }
}

/*
 * Helper function that recursively searches for the minimax
 * solution. Returns a pair including the optimized score and
 * the best move.
 */
pair<int, Move*> Player::minimaxHelper(int depth, Board *b, Side s) {
    int best_score = (s == _side) ? (INT_MIN) : (INT_MAX);
    Move *best = NULL;
    
    // Base Case: Just evaluate board
    if (depth == 0) {
	return make_pair(this->evaluate(b), best);
    }
    else {
	vector<Move *> moves;
	b->getPossibleMoves(s, moves);

	// There are no more possible moves
	if (moves.empty()) {
	    return make_pair(this->evaluate(b), best);
	}
	for (vector<Move *>::iterator it = moves.begin(); it != moves.end(); ++it) {
	    Board *nextBoard = b->copy();
	    
	    nextBoard->doMove(*it, s);
	    if (s == _side) {
		// Wants to maximize the possible score
		pair<int, Move*> score = this->minimaxHelper(depth - 1, nextBoard, _opponentSide);
		if (score.first >= best_score) {
		    best_score = score.first;
		    best = *it;
		}
	    }
	    else {
		// Opponent wants to minimize the possible score
		pair<int, Move*> score = this->minimaxHelper(depth - 1, nextBoard, _side);
		if (score.first <= best_score) {
		    best_score = score.first;
		    best = *it;
		}
	    }
	    delete nextBoard;
	}
    }
    return make_pair(best_score, best);
}
