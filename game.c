//
// Created by Michael on 10.11.2024.
//

#include "game.h"
#include "constants.h"

int handleRock(int secondPlayerChoise) {
    if (secondPlayerChoise == SCISSORS_VALUE || secondPlayerChoise == LIZARD_VALUE) {
        return VICOTRY_VALUE;
    } else {
        return DEFEAT_VALUE;
    }
}

int handlePaper(int secondPlayerChoise) {
    if (secondPlayerChoise == ROCK_VALUE || secondPlayerChoise == SPOCK_VALUE) {
        return VICOTRY_VALUE;
    } else {
        return DEFEAT_VALUE;
    }
}

int handleScissors(int secondPlayerChoise) {
    if (secondPlayerChoise == PAPER_VALUE || secondPlayerChoise == LIZARD_VALUE) {
        return VICOTRY_VALUE;
    } else {
        return DEFEAT_VALUE;
    }
}

int handleLizard(int secondPlayerChoise) {
    if (secondPlayerChoise == PAPER_VALUE || secondPlayerChoise == SPOCK_VALUE) {
        return VICOTRY_VALUE;
    } else {
        return DEFEAT_VALUE;
    }
}

int handleSpock(int secondPlayerChoise) {
    if (secondPlayerChoise == ROCK_VALUE || secondPlayerChoise == SCISSORS_VALUE) {
        return VICOTRY_VALUE;
    } else {
        return DEFEAT_VALUE;
    }
}

int resultOfGameRound(int firstPlayerChoise, int secondPlayerChoise) {
    int navrat = FAILURE_VALUE;
    if (firstPlayerChoise == secondPlayerChoise) {
        navrat = STALEMATE_VALUE;
    } else if (firstPlayerChoise == ROCK_VALUE) {
        navrat = handleRock(secondPlayerChoise);
    } else if (firstPlayerChoise == PAPER_VALUE) {
        navrat = handlePaper(secondPlayerChoise);
    } else if (firstPlayerChoise == SCISSORS_VALUE) {
        navrat = handleScissors(secondPlayerChoise);
    } else if (firstPlayerChoise == LIZARD_VALUE) {
        navrat = handleLizard(secondPlayerChoise);
    } else if (firstPlayerChoise == SPOCK_VALUE) {
        navrat = handleSpock(secondPlayerChoise);
    }
    return navrat;
}