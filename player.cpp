#include "player.h"

bitset<64> EDGES = bitset<64>(0xff818181818181ffULL);
bitset<64> CORNERS = bitset<64>(0x8100000000000081ULL);

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

    this->computeOpening();
    std::cerr << "Done Initialization" << std::endl;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    _table.clear();
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
    Move *m = NULL;
    // If a time limit is specified, the AI will do iterative deepening to 
    // use up as much time as safely possible.
    if (msLeft > 0) {
    	clock_t start = clock();

    	// 500 may be an overestimate. Can optimize later
    	double time_allowed = (msLeft) / TIMESPLIT;
    	
    	int depth = 2;

    	// While there is still time left, it will compute one depth further. While
    	// it repeats some calculations, that fact that we have a transposition table
    	// should minimize the time wasted. 
    	while ( (double)(clock() - start) / (CLOCKS_PER_SEC / 1000) < time_allowed) {
    	    m = (testingMinimax) ? 
    		(this->findMinimaxMove(2)) : (this->findMinimaxMove(depth++));
    	}
    	// FOR DEBUGGING
    	std::cerr << depth << std::endl;
    	
    } else {
    	m = (testingMinimax) ? 
    	    (this->findMinimaxMove(2)) : (this->findMinimaxMove(MINIMAXDEPTH));
    }
    _board->doMove(m, _side);
    return m;
}

/*
 * Returns the first available move that the AI finds.
 */
Move *Player::findFirstMove() {
    bitset<64> moves = _board->getPossibleMoves(_side);
    if (moves.any()) {
        for (int i = 0; i < 64; i++) {
            if (moves.test(i)) {
                return new Move(i % 8, i / 8);
            }
        }
    }
    return NULL;
}

/*
 * Uses the minimax algorithm to look for the best move
 */
Move *Player::findMinimaxMove(int depth) {
    int alpha = INT_MIN;
    int beta = INT_MAX;
    
    bitset<64> moves = _board->getPossibleMoves(_side);
    
    Move *best = new Move(0,0); // Stores best move
    Move current_move = Move(0,0);
    Board *next_board;
    int score;
    
    for (int i = 0; i < 64; i++) {
        if (moves.test(i)) {
            current_move = Move(i % 8, i / 8);
            next_board = _board->copy();
            next_board->doMove(&current_move, _side);
            
            score = this->minimaxHelper(depth - 1, next_board, _opponentSide, alpha, beta);
            if (score >= alpha) {
                alpha = score;
                best->setX(current_move.getX());
                best->setY(current_move.getY());
            }
            
        }
    }
    return best;
}

/*
 * Helper function that recursively searches for the minimax
 * solution. Returns a pair including the optimized score alpha/beta
 * and the best move.
 */
int Player::minimaxHelper(int depth, Board *b, Side s, int alpha, int beta) {
    // Compute the bitsets for ai and opponent sides, and converts
    // to a 64-bit integer
    unsigned long long black = (b->black).to_ullong(), total = (b->taken).to_ullong();
            
    // Hash value is simply concatenation of 2 integers
    // TODO: hash minimax helper instead?
    string hash = to_string(black) + ", " + to_string(total) + ", ";
    if (_table.find(hash + to_string(depth)) != _table.end()) {
        return _table[hash];
    }
        
    int answer;
    
    // Base Case: Just evaluate board
    if (depth == 0) {
	    answer = this->evaluate(b);
        _table[hash + to_string(0)] = answer;
        return answer;
    }
    bitset<64> moves = b->getPossibleMoves(s);
    
    // There are no more possible moves
    if (moves.none()) {
        return this->evaluate(b);
    }
    
    Move current_move = Move(0,0);
    Board *next_board;
    int score;
    
    if (s == _side) {
        alpha = INT_MIN;
        for (int i = 0; i < 64; i++) {
            if (moves.test(i)) {
                current_move = Move(i % 8, i / 8);
                next_board = b->copy();
                next_board->doMove(&current_move, s);
                
                //Wants to maximize the possible score
                score = this->minimaxHelper(depth - 1, next_board, _opponentSide, alpha, beta);
                delete next_board;
                alpha = max(alpha, score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
        answer = alpha;
    } else {
        beta = INT_MAX;
        for (int i = 0; i < 64; i++) {
            if (moves.test(i)) {
                current_move = Move(i % 8, i / 8);
                next_board = b->copy();
                next_board->doMove(&current_move, s);
                
                //Wants to maximize the possible score
                score = this->minimaxHelper(depth - 1, next_board, _side, alpha, beta);
                delete next_board;
                beta = min(beta, score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
        answer = beta;
    }
    
    // Keeps transposition table at fixed size
    while (_table.size() >= 100000) {
        // Removes the first key, since it is probably the furthest 
        // away from the current game state
        _table.erase(_table.begin());
    }
    
    for (int d = 0; d <= depth; d++) {
        _table[hash + to_string(d)] = answer;
    }
    return answer;
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
	    int score = 0;
        bitset<64> white = (b->taken & ~(b->black));
        
        // Check for winning board
        if ((b->black).count() == 0) {
            return (_side == BLACK) ? INT_MIN : INT_MAX;
        } else if (white.count() == 0) {
            return (_side == WHITE) ? INT_MIN : INT_MAX;
        }
        
        // Coin count
        score += (b->black).count() - white.count();
        
        // Edges and corners
        score += EDGEWEIGHT * (b->black & EDGES).count();
        score += CORNERWEIGHT * (b->black & CORNERS).count();
        score -= EDGEWEIGHT * (white & EDGES).count();
        score -= CORNERWEIGHT * (white & CORNERS).count();
        
        // Mobility
//        bitset<64> next_moves;
//        next_moves = b->getPossibleMoves(BLACK);
//        score += next_moves.count();
//        next_moves = b->getPossibleMoves(WHITE);
//        score -= next_moves.count();
        
        // Stability
//        score += STABILITYWEIGHT * b->getStablePieceCount(BLACK);
//        score -= STABILITYWEIGHT * b->getStablePieceCount(WHITE);
        
        if (_side == WHITE) {
            score *= -1;
        }
	    return score;
    }
}

/*
 * Stores the score of positions early in the game so they can be 
 * looked up quickly.
 */
void Player::computeOpening() {
    //vector<pair<Side, Board *> > positions;
    //positions.push_back(make_pair(_side, _board->copy()));
    
    // Computes initial 25000 board positions, and stores their 
    // score in the transposition table
    
    // TODO: if time is limited, don't fill table all the way here
     
    findMinimaxMove(8);
       
//    while (_table.size() < 25000) {
//	    pair<Side, Board *> curr = positions.front();
//	    this->evaluate(curr.second);
//	
//    	positions.erase(positions.begin());
//
//        bitset<64> moves = curr.second->getPossibleMoves(curr.first);
//        Side next_side = (curr.first == BLACK) ? (WHITE) : (BLACK);
//        Move current_move = Move(0,0);
//        
//        for (int i = 0; i < 64; i++) {
//            if (moves.test(i)) {
//                current_move = Move(i % 8, i / 8);
//                Board *next_board = curr.second->copy();
//                next_board->doMove(&current_move, curr.first);
//                positions.push_back(make_pair(next_side, next_board));
//            }
//        }
//        delete curr.second;
//    }
//    
//    while (!positions.empty()) {
//	pair<Side, Board *> it = positions.back();
//	positions.pop_back();
//	delete it.second;
//    }
	
}

