#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <climits>

#include "common.h"
#include "board.h"
using namespace std;

class Player {

private:
    Board* _board;
    Side _side;
    Side _opponentSide;

    Move *findFirstMove();
    Move *findMinimaxMove(int depth);
    
    int evaluate(Board *b);
    
    pair<int, Move*> minimaxHelper(int depth, Board *b, Side s);
    
public:
    Player(Side side);
    ~Player();
    
    Move *doMove(Move *opponentsMove, int msLeft);
    inline void setBoard(char data[]) { _board->setBoard(data); }
    
    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
