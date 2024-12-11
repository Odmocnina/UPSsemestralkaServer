//
// Created by Michael on 10.11.2024.
//

#include <stdio.h>

#include "game.h"
#include "constants.h"
#include "gameObjects.h"

int handleRock(int firstPlayerTurn) {
    printf("v kamenu: %d\n", firstPlayerTurn);
    if (firstPlayerTurn == SCISSORS_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handlePaper(int firstPlayerTurn) {
    printf("v papiru: %d\n", firstPlayerTurn);
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleScissors(int firstPlayerTurn) {
    printf("v nuzky: %d\n", firstPlayerTurn);
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleLizard(int firstPlayerTurn) {
    printf("v tapirovi: %d\n", firstPlayerTurn);
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int handleSpock(int firstPlayerTurn) {
    printf("v spock: %d\n", firstPlayerTurn);
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SCISSORS_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

int resultOfGameRound(int firstPlayerTurn, int secondPlayerTurn) {
    int navrat = FAILURE_VALUE;
    printf("first player: %d second player %d\n", firstPlayerTurn, secondPlayerTurn);
    if (firstPlayerTurn == secondPlayerTurn) {
        navrat = STALEMATE_VALUE;
    } else if (secondPlayerTurn == ROCK_VALUE) {
        navrat = handleRock(firstPlayerTurn);
    } else if (secondPlayerTurn == PAPER_VALUE) {
        navrat = handlePaper(firstPlayerTurn);
    } else if (secondPlayerTurn == SCISSORS_VALUE) {
        navrat = handleScissors(firstPlayerTurn);
    } else if (secondPlayerTurn == LIZARD_VALUE) {
        navrat = handleLizard(firstPlayerTurn);
    } else if (secondPlayerTurn == SPOCK_VALUE) {
        navrat = handleSpock(firstPlayerTurn);
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
    printf("vysledek hry: %d\n", result);
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