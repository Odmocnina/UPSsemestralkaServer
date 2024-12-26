//
// Created by Michael on 12.11.2024.
//

#include "gameObjects.h"
#include "constants.h"
#include "postMan.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct player players[MAX_NUMBER_OF_PLAYERS];

struct lobby runningGames[MAX_NUMBER_OF_PLAYERS / 2];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void inicilazePlayerArray() {
    pthread_mutex_lock(&lock);
    printf("Inicializuji pole hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        players[i].state = FREE_POSITION;

        players[i].index = FREE_POSITION;
    }
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        runningGames[i].indexOfPlayer1 = FREE_POSITION;

        runningGames[i].indexOfPlayer2 = FREE_POSITION;

    }
    pthread_mutex_unlock(&lock);
}

int addPlayerToGamersArray(char *name, int clientSocket) {
    pthread_mutex_lock(&lock);
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
            players[i].numberOfPings = 0;
            players[i].numberOfPongs = 0;
            navrat = i;
            found = true;
        }
        i = i + 1;
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getFreePlayer(int indexOfConnectedPlayer) {
    pthread_mutex_lock(&lock);
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
    pthread_mutex_unlock(&lock);
    return navrat;
}

void updatePlayerArray(int index) {
    for (int i = index; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {

    }
}

void removePlayerFromGamersArray(int index) {
    pthread_mutex_lock(&lock);
    players[index].state = FREE_POSITION;
    players[index].name[0] = '\0';
    players[index].index = FREE_POSITION;
    pthread_mutex_unlock(&lock);
}

void printPlayerArray() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

void printPlayerArray2() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
        printf("lobby: %d\n", players[i].game);
        printf("turn: %d\n", players[i].turn);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

void printGamesArray() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu her\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        printf("id1: %d\n", runningGames[i].indexOfPlayer1);
        printf("id2: %d\n", runningGames[i].indexOfPlayer2);
        printf("kola: %d\n", runningGames[i].numberOfPlayedRounds);
        printf("1s: %d\n", runningGames[i].indexOfPlayer1);
        printf("2s: %d\n", runningGames[i].indexOfPlayer2);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

int getScoreOfFirstPlayer(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].firstPlayerScore;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getScoreOfSecondPlayer(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].secondPlayerScore;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getNumberOfRounds(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].numberOfPlayedRounds;
    pthread_mutex_unlock(&lock);
    return navrat;
}

void updateGameScore(int game, int player1Gain, int player2Gain, int stalemateGain) {
    pthread_mutex_lock(&lock);
    runningGames[game].firstPlayerScore = runningGames[game].firstPlayerScore + player1Gain;
    runningGames[game].secondPlayerScore = runningGames[game].secondPlayerScore + player2Gain;
    runningGames[game].numberOfStalemates = runningGames[game].numberOfStalemates + stalemateGain;
    runningGames[game].numberOfPlayedRounds = runningGames[game].numberOfPlayedRounds + 1;
    pthread_mutex_unlock(&lock);
}

void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer, int *game) {
    pthread_mutex_lock(&lock);
    int i = 0;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS / 2)) {
        if (runningGames[i].indexOfPlayer1 == FREE_POSITION) {
            found = true;
            runningGames[i].indexOfPlayer1 = indexFirstPlayer;
            runningGames[i].indexOfPlayer2 = indexSecondPlayer;
            runningGames[i].numberOfPlayedRounds = 0;
            runningGames[i].firstPlayerScore = 0;
            runningGames[i].secondPlayerScore = 0;
            runningGames[i].numberOfStalemates = 0;
            players[indexFirstPlayer].state = IN_GAME_VALUE;
            players[indexSecondPlayer].state = IN_GAME_VALUE;
            players[indexFirstPlayer].turn = TURN_NOT_PICKED_YET;
            players[indexSecondPlayer].turn = TURN_NOT_PICKED_YET;
            *game = i;
            players[indexFirstPlayer].game = i;
            players[indexSecondPlayer].game = i;
        }
        i = i + 1;
    }
    pthread_mutex_unlock(&lock);
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
        addNewRunningGame(freePlayer, id, &navrat);
        navrat = true;
    }
    return navrat;
}

int getIdOfOpponent(int game, int id) {
    pthread_mutex_lock(&lock);
    if (runningGames[game].indexOfPlayer2 == id) {
        int navrat = runningGames[game].indexOfPlayer1;
        pthread_mutex_unlock(&lock);
        return navrat;
    } else {
        int navrat = runningGames[game].indexOfPlayer2;
        pthread_mutex_unlock(&lock);
        return navrat;
    }
}

int getSocketOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].clientSocket;
    pthread_mutex_unlock(&lock);
    return navrat;
}

void setTurnOfPlayer(int indexOfPlayer, int turn) {
    pthread_mutex_lock(&lock);
    players[indexOfPlayer].turn = turn;
    pthread_mutex_unlock(&lock);
    /*if (runningGames[game].whoTurnedSooner == NOONE_PLAYED_YET) {
        runningGames[game].whoTurnedSooner = indexOfPlayer;
    }*/
}

void unSetTurnOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    players[indexOfPlayer].turn = TURN_NOT_PICKED_YET;
    pthread_mutex_unlock(&lock);
}

bool bothPlayersHaveTurn(int indexOfGame) {
    pthread_mutex_lock(&lock);
    if (indexOfGame == FAILURE_VALUE) {
        pthread_mutex_unlock(&lock);
        return false;
    }
    int turnOfFirstPlayer = players[runningGames[indexOfGame].indexOfPlayer1].turn;
    int turnOfSecondPlayer = players[runningGames[indexOfGame].indexOfPlayer2].turn;
    pthread_mutex_unlock(&lock);
    return turnOfFirstPlayer != TURN_NOT_PICKED_YET && turnOfSecondPlayer != TURN_NOT_PICKED_YET;
}

void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer) {
    pthread_mutex_lock(&lock);
    *firstPlayer = runningGames[indexOfGame].indexOfPlayer1;
    *secondPlayer = runningGames[indexOfGame].indexOfPlayer2;
    pthread_mutex_unlock(&lock);
}

int getTurnOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    int navrat = players[indexOfPlayer].turn;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getGameOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    int navrat = players[indexOfPlayer].game;
    pthread_mutex_unlock(&lock);
    return navrat;
}

bool isFirstPlayer(int id, int game) {
    pthread_mutex_lock(&lock);
    bool navrat = runningGames[game].indexOfPlayer1 == id;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int freePlayer(int id) {
    pthread_mutex_lock(&lock);
    if (players[id].state == WAITING_VALUE) {
        pthread_mutex_unlock(&lock);
        return FAILURE_VALUE;
    }
    players[id].state = WAITING_VALUE;
    players[id].turn = TURN_NOT_PICKED_YET;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int unsetGame(int game) {
    pthread_mutex_lock(&lock);
    if (runningGames[game].indexOfPlayer1 == FREE_POSITION && runningGames[game].indexOfPlayer2 == FREE_POSITION) {
        pthread_mutex_unlock(&lock);
        return FAILURE_VALUE;
    }
    runningGames[game].indexOfPlayer1 = FREE_POSITION;
    runningGames[game].indexOfPlayer2 = FREE_POSITION;
    runningGames[game].firstPlayerScore = 0;
    runningGames[game].secondPlayerScore = 0;
    runningGames[game].numberOfPlayedRounds = 0;
    runningGames[game].numberOfStalemates = 0;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int getStalemates(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].numberOfStalemates;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int sendPingToAllPlayers() {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        if (players[i].state != FREE_POSITION) {
            sendPing(players[i].clientSocket);
        }
    }
    return SUCCESS_VALUE;
}

int setPingOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = FAILURE_VALUE;
    if (players[id].state != FREE_POSITION) {
        players[id].numberOfNotAnwseredPings = 0;
        navrat = SUCCESS_VALUE;
    }
    pthread_mutex_unlock(&lock);
}

bool checkName(char *name) {
    pthread_mutex_lock(&lock);
    int i = 0;
    bool navrat = false;
    while (i < MAX_NUMBER_OF_PLAYERS && !navrat) {
        if (strcmp(players[i].name, name) == STRINGS_ARE_SAME) {
            navrat = true;
        } else {
            i = i + 1;
        }
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getNumberOfPings(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].numberOfPings;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getNumberOfPongs(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].numberOfPongs;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int setNumberOfPings(int id, int numberOfPings) {
    pthread_mutex_lock(&lock);
    players[id].numberOfPings = numberOfPings;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int setNumberOfPongs(int id, int numberOfPongs) {
    pthread_mutex_lock(&lock);
    players[id].numberOfPongs = numberOfPongs;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}
//bool getWhoIsPlayer(int id) {
//    return players[id].makerOfGame;
//}
