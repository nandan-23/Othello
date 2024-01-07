#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <list>
#include <climits>
#include <vector>

using namespace std;
using namespace Desdemona;

class MyBot: public OthelloPlayer {
public:
    MyBot(Turn turn);
    virtual Move play(const OthelloBoard& board);
    int alphaBeta(const OthelloBoard& board, int depth, int alpha, int beta, Turn currentTurn);
    int evaluateBoard(const OthelloBoard& board, Turn turn);

private:
    const int maxDepth = 4; 
    vector<vector<int>> positionalWeights;
    int mobility(const OthelloBoard& board, Turn turn);
};

MyBot::MyBot(Turn turn) : OthelloPlayer(turn) {
    positionalWeights = {
        {20, -3, 11,  8,  8, 11, -3, 20},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {11, -4,  2,  2,  2,  2, -4, 11},
        { 8,  1,  2, -3, -3,  2,  1,  8},
        { 8,  1,  2, -3, -3,  2,  1,  8},
        {11, -4,  2,  2,  2,  2, -4, 11},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {20, -3, 11,  8,  8, 11, -3, 20}
    };
}

Move MyBot::play(const OthelloBoard& board) {
    list<Move> moves = board.getValidMoves(turn);
    Move bestMove = *moves.begin();
    int alpha = INT_MIN;
    int beta = INT_MAX;

    for (Move move : moves) {
        OthelloBoard tempBoard = board;
        tempBoard.makeMove(turn, move);
        int value = alphaBeta(tempBoard, maxDepth - 1, alpha, beta, other(turn));
        if (value > alpha) {
            alpha = value;
            bestMove = move;
        }
    }

    return bestMove;
}

int MyBot::alphaBeta(const OthelloBoard& board, int depth, int alpha, int beta, Turn currentTurn) {
    if (depth == 0) {
        return evaluateBoard(board, currentTurn);
    }

    list<Move> moves = board.getValidMoves(currentTurn);

    if (currentTurn == turn) {
        for (Move move : moves) {
            OthelloBoard tempBoard = board;
            tempBoard.makeMove(currentTurn, move);
            int value = alphaBeta(tempBoard, depth - 1, alpha, beta, other(currentTurn));
            alpha = max(alpha, value);
            if (beta <= alpha)
                break;
        }
        return alpha;
    } else {
        for (Move move : moves) {
            OthelloBoard tempBoard = board;
            tempBoard.makeMove(currentTurn, move);
            int value = alphaBeta(tempBoard, depth - 1, alpha, beta, other(currentTurn));
            beta = min(beta, value);
            if (beta <= alpha)
                break;
        }
        return beta;
    }
}

int MyBot::evaluateBoard(const OthelloBoard& board, Turn turn) {
    int score = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board.get(i, j) == turn) {
                score += positionalWeights[i][j];
            } else if (board.get(i, j) == other(turn)) {
                score -= positionalWeights[i][j];
            }
        }
    }

    score += mobility(board, turn);

    return score;
}

int MyBot::mobility(const OthelloBoard& board, Turn turn) {
    int myMoves = board.getValidMoves(turn).size();
    int opponentMoves = board.getValidMoves(other(turn)).size();
    if (myMoves + opponentMoves != 0)
        return 10 * (myMoves - opponentMoves) / (myMoves + opponentMoves);
    return 0;
}

extern "C" {
    OthelloPlayer* createBot(Turn turn) {
        return new MyBot(turn);
    }

    void destroyBot(OthelloPlayer* bot) {
        delete bot;
    }
}