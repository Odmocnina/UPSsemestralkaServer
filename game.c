//
// Created by Michael on 10.11.2024.
//

#include <stdio.h>

#include "game.h"
#include "constants.h"
#include "gameObjects.h"

int handleRock(int firstPlayerTurn) {
    if (firstPlayerTurn == SCISSORS_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handlePaper(int firstPlayerTurn) {
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleScissors(int firstPlayerTurn) {
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleLizard(int firstPlayerTurn) {
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleSpock(int firstPlayerTurn) {
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SCISSORS_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int resultOfGameRound(int firstPlayerTurn, int secondPlayerTurn) {
    int navrat = FAILURE_VALUE;
    if (firstPlayerTurn == secondPlayerTurn) {
        navrat = STALEMATE_VALUE;
    } else if (firstPlayerTurn == ROCK_VALUE) {
        navrat = handleRock(secondPlayerTurn);
    } else if (firstPlayerTurn == PAPER_VALUE) {
        navrat = handlePaper(secondPlayerTurn);
    } else if (firstPlayerTurn == SCISSORS_VALUE) {
        navrat = handleScissors(secondPlayerTurn);
    } else if (firstPlayerTurn == LIZARD_VALUE) {
        navrat = handleLizard(secondPlayerTurn);
    } else if (firstPlayerTurn == SPOCK_VALUE) {
        navrat = handleSpock(secondPlayerTurn);
    }
    return navrat;
}


void handleGame(int indexOfGame, char *messageForFirstPlayer, char *messageForSecondPlayer) {
    int indexOfFirstPlayer;
    int indexOfSecondPlayer;
    getIndexOfPlayers(indexOfGame, &indexOfFirstPlayer, &indexOfSecondPlayer);
    printf("indexy hracu: %d %d\n", indexOfFirstPlayer, indexOfSecondPlayer);
    int firstPlayerTurn = getTurnOfPlayer(indexOfFirstPlayer);
    int secondPlayerTurn = getTurnOfPlayer(indexOfSecondPlayer);
    int result = resultOfGameRound(firstPlayerTurn, secondPlayerTurn);
    if (result == STALEMATE_VALUE) {
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:\n", "gameResult", "s", secondPlayerTurn);
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:\n", "gameResult", "s", firstPlayerTurn);
    } else if (result == FIRST_PLAYER_WON) {
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:\n", "gameResult", "w", secondPlayerTurn);
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:\n", "gameResult", "l", firstPlayerTurn);
    } else if (result == SECOND_PLAYER_WON) {
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:\n", "gameResult", "l", secondPlayerTurn);
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:\n", "gameResult", "w", firstPlayerTurn);
    } else {
        printf("Chyba pri zracovavani hry\n");
    }
}