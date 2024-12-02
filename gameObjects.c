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
        printf("hledam");
        if (players[i].state == WAITING_VALUE && i != indexOfConnectedPlayer) {
            found = true;
            navrat = i;
        }
        i = i + 1;
    }
    printf("navrat %d\n", navrat);
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

int attemptGameStart(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    int id = atoi(strtok(NULL, ":"));
    int freePlayer = getFreePlayer(id);
    if (freePlayer != FAILURE_VALUE) {
        printf("vytvarim hru\n");
        addNewRunningGame(id, freePlayer);
    }
    return freePlayer;
}

void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer) {
    int i = 0;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS / 2)) {
        if (runningGames[i].indexOfPlayer1 == FREE_POSITION) {
            found = true;
            runningGames[i].indexOfPlayer1 = indexFirstPlayer;
            runningGames[i].indexOfPlayer2 = indexSecondPlayer;
        }
        i = i + 1;
    }
}

int getSocketOfPlayer(int indexOfPlayer) {
    return players[indexOfPlayer].clientSocket;
}
