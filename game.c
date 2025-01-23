//
// Created by Michael on 10.11.2024.
//

#include <stdio.h>

#include "game.h"
#include "constants.h"
#include "gameObjects.h"

/**
 * Funkce na zhodnoceni vysledku hry pri volbe kamen
 *
 * @param firstPlayerTurn volba prvniho hrace
 * **/
int handleRock(int firstPlayerTurn) {
    if (firstPlayerTurn == SCISSORS_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}


/**
 * Funkce na zhodnoceni vysledku hry pri volbe papir
 *
 * @param firstPlayerTurn volba prvniho hrace
 * **/
int handlePaper(int firstPlayerTurn) {
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

/**
 * Funkce na zhodnoceni vysledku hry pri volbe nuzky
 *
 * @param firstPlayerTurn volba prvniho hrace
 * **/
int handleScissors(int firstPlayerTurn) {
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == LIZARD_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

/**
 * Funkce na zhodnoceni vysledku hry pri volbe tapir
 *
 * @param firstPlayerTurn volba prvniho hrace
 * **/
int handleLizard(int firstPlayerTurn) {
    if (firstPlayerTurn == PAPER_VALUE || firstPlayerTurn == SPOCK_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}

/**
 * Funkce na zhodnoceni vysledku hry pri volbe spock
 *
 * @param firstPlayerTurn volba prvniho hrace
 * **/
int handleSpock(int firstPlayerTurn) {
    if (firstPlayerTurn == ROCK_VALUE || firstPlayerTurn == SCISSORS_VALUE) {
        return SECOND_PLAYER_WON;
    } else {
        return FIRST_PLAYER_WON;
    }
}




/**
 * funkce na zjsiteni vysledku hry ze dvou tahu
 *
 *
 * @param firstPlayerTurn tah prvniho hrace
 * @param secondPlayerTurn tah druheho hrace
 * @return vysledek hry
 **/
int resultOfGameRound(int firstPlayerTurn, int secondPlayerTurn) {
    int navrat = FAILURE_VALUE;
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


/**
 * na smasotantou oblosuhu hry kdyz oba hraci zvoli svuj tah
 *
 *
 * @param indexOfGame index hry, kterou vyhodnocujeme
 * @param messageForFirstPlayer zprv pro prvniho hrace
 * @param messageForSecondPlayer zprv pro druheho hrace
 **/
void handleGame(int indexOfGame, char *messageForFirstPlayer, char *messageForSecondPlayer) {
    int indexOfFirstPlayer;
    int indexOfSecondPlayer;
    getIndexOfPlayers(indexOfGame, &indexOfFirstPlayer, &indexOfSecondPlayer);
    //printGamesArray();
    int firstPlayerTurn = getTurnOfPlayer(indexOfFirstPlayer);
    int secondPlayerTurn = getTurnOfPlayer(indexOfSecondPlayer);
    int result = resultOfGameRound(firstPlayerTurn, secondPlayerTurn);
    if (result == STALEMATE_VALUE) {
        updateGameScore(indexOfGame, 0, 0, 1);
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "s", secondPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfFirstPlayer(indexOfGame));
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "s", firstPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfSecondPlayer(indexOfGame));
        setWinnerLastRound(indexOfGame, STALEMATE_VALUE);
    } else if (result == FIRST_PLAYER_WON) {
        updateGameScore(indexOfGame, 1, 0, 0);
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "w", secondPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfFirstPlayer(indexOfGame));
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "l", firstPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfSecondPlayer(indexOfGame));
        setWinnerLastRound(indexOfGame, indexOfFirstPlayer);
    } else if (result == SECOND_PLAYER_WON) {
        updateGameScore(indexOfGame, 0, 1, 0);
        sprintf(messageForFirstPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "l", secondPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfFirstPlayer(indexOfGame));
        sprintf(messageForSecondPlayer, "Mess:%s:%s:%d:%d:%d:%d:\n", "gameResult", "w", firstPlayerTurn
                , getNumberOfRounds(indexOfGame), getStalemates(indexOfGame), getScoreOfSecondPlayer(indexOfGame));
        setWinnerLastRound(indexOfGame, indexOfSecondPlayer);
    } else {
        printf("Chyba pri zracovavani hry\n");
    }
}