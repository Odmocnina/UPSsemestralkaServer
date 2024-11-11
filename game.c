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
    if (firstPlayerChoise == secondPlayerChoise) {
        return STALEMATE_VALUE;
    } else if (firstPlayerChoise == ROCK_VALUE) {
        return handleRock(secondPlayerChoise);
    } else if (firstPlayerChoise == PAPER_VALUE) {
        return handlePaper(secondPlayerChoise);
    } else if (firstPlayerChoise == SCISSORS_VALUE) {
        return handleScissors(secondPlayerChoise);
    } else if (firstPlayerChoise == LIZARD_VALUE) {
        return handleLizard(secondPlayerChoise);
    } else if (firstPlayerChoise == SPOCK_VALUE) {
        return handleSpock(secondPlayerChoise);
    } else {
        return FAILURE_VALUE;
    }
}