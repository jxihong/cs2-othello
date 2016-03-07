#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <climits>

#include "common.h"
#include "board.h"

#define MINIMAXDEPTH 6
#define EDGEWEIGHT 2
#define CORNERWEIGHT 8

using namespace std;

class Player {

private:
    Board* _board;
    Side _side;
    Side _opponentSide;
    
    unordered_map<string, int> _table; // transposition table
   
    Move *findFirstMove();
    Move *findMinimaxMove(int depth);
    pair<int, Move*> minimaxHelper(int depth, Board *b, Side s);
    
    void computeOpening();
    int evaluate(Board *b);
public:
    Player(Side side);
    ~Player();
    
    Move *doMove(Move *opponentsMove, int msLeft);
    inline void setBoard(char data[]) { _board->setBoard(data); }
    
    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
