//
// Created by Michael on 12.11.2024.
//

#include "gameObjects.h"
#include "constants.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct player players[MAX_NUMBER_OF_PLAYERS];

struct lobby runningGames[MAX_NUMBER_OF_PLAYERS / 2];

void inicilazePlayerArray() {
    printf("Inicializuji pole hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        players[i].state = FREE_POSITION;

        players[i].index = FREE_POSITION;
    }
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        runningGames[i].indexOfPlayer1 = FREE_POSITION;

        runningGames[i].indexOfPlayer2 = FREE_POSITION;
    }
}

int addPlayerToGamersArray(char *name, int clientSocket) {
    int i = 0;
    bool found = false;
    int navrat = FAILURE_VALUE;
    while (!found && i < MAX_NUMBER_OF_PLAYERS) {
        if (players[i].state == FREE_POSITION) {
            strncpy(players[i].name, name, sizeof(players[i].name));
            players[i].name[MAX_NAME_LENGTH - 1] = '\0';
            players[i].state = WAITING_VALUE;
            players[i].index = i;
            players[i].clientSocket = clientSocket;
            navrat = i;
            found = true;
        }
        i = i + 1;
    }
    return navrat;
}

int getFreePlayer(int indexOfConnectedPlayer) {
    int i = 0;
    int navrat = FAILURE_VALUE;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS)) {
        if (players[i].state == WAITING_VALUE && i != indexOfConnectedPlayer) {
            found = true;
            navrat = i;
        }
        i = i + 1;
    }
    return navrat;
}

void updatePlayerArray(int index) {
    for (int i = index; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {

    }
}

void removePlayerFromGamersArray(int index) {
    players[index].state = FREE_POSITION;
    players[index].name[0] = '\0';
    players[index].index = FREE_POSITION;
}

void printPlayerArray() {
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
    }
    printf("\n");
}

void printPlayerArray2() {
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
        printf("game: %d\n", players[i].game);
        printf("turn: %d\n", players[i].turn);
    }
    printf("\n");
}

void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer, int *game) {
    int i = 0;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS / 2)) {
        if (runningGames[i].indexOfPlayer1 == FREE_POSITION) {
            found = true;
            runningGames[i].indexOfPlayer1 = indexFirstPlayer;
            runningGames[i].indexOfPlayer2 = indexSecondPlayer;
            players[indexFirstPlayer].state = IN_GAME_VALUE;
            players[indexSecondPlayer].state = IN_GAME_VALUE;
            players[indexFirstPlayer].turn = TURN_NOT_PICKED_YET;
            players[indexSecondPlayer].turn = TURN_NOT_PICKED_YET;
            *game = i;
            players[indexFirstPlayer].game = i;
            players[indexSecondPlayer].game = i;
            players[indexFirstPlayer].makerOfGame = true;
            players[indexSecondPlayer].makerOfGame = false;
        }
        i = i + 1;
    }
}

bool attemptGameStart(int id) {
    printf("Pokus o zanuti hry\n");
    //strtok(message, ":");
    //strtok(NULL, ":");
    //int id = atoi(strtok(NULL, ":"));
    //printf("id: %d\n", id);
    int freePlayer = getFreePlayer(id);
    int navrat = false;
    if (freePlayer != FAILURE_VALUE) {
        addNewRunningGame(id, freePlayer, &navrat);
        navrat = true;
    }
    return navrat;
}

int getIdOfOpponent(int indexOfGame, bool who) {
    if (who == false) {
        return players[runningGames[indexOfGame].indexOfPlayer2].index;
    }
    if (who == true) {
        return players[runningGames[indexOfGame].indexOfPlayer1].index;
    }
}

int getSocketOfPlayer(int indexOfGame, bool who) {
    if (who == false) {
        return players[runningGames[indexOfGame].indexOfPlayer2].clientSocket;
    }
    if (who == true) {
        return players[runningGames[indexOfGame].indexOfPlayer1].clientSocket;
    }
}

void setTurnOfPlayer(int indexOfPlayer, int turn) {
    printf("nastavuju tah\n");
    players[indexOfPlayer].turn = turn;
}

bool bothPlayersHaveTurn(int indexOfGame) {
    if (indexOfGame == FAILURE_VALUE) {
        printf("v saniti checku");
        return false;
    }
    int turnOfFirstPlayer = players[runningGames[indexOfGame].indexOfPlayer1].turn;
    int turnOfSecondPlayer = players[runningGames[indexOfGame].indexOfPlayer2].turn;
    printf("turn1: %d turn2: %d\n", turnOfFirstPlayer, turnOfSecondPlayer);
    return turnOfFirstPlayer != TURN_NOT_PICKED_YET && turnOfSecondPlayer != TURN_NOT_PICKED_YET;
}

void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer) {
    *firstPlayer = runningGames[indexOfGame].indexOfPlayer1;
    *secondPlayer = runningGames[indexOfGame].indexOfPlayer2;
}

int getTurnOfPlayer(int indexOfPlayer) {
    printf("turn of player %d: %d", indexOfPlayer, players[indexOfPlayer].turn);
    return players[indexOfPlayer].turn;
}

int getGameOfPlayer(int indexOfPlayer) {
    return players[indexOfPlayer].game;
}

bool getWhoIsPlayer(int id) {
    return players[id].makerOfGame;
}
