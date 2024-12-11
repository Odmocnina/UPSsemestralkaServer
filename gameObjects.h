//
// Created by Michael on 12.11.2024.
//

#ifndef SERVER_GAMEOBJECTS_H
#define SERVER_GAMEOBJECTS_H

#include <stdbool.h>

#include "constants.h"
#include "player.h"

struct lobby {
    int indexOfPlayer1;
    int indexOfPlayer2;
    int whoTurnedSooner;
};

void inicilazePlayerArray();


int addPlayerToGamersArray(char *name, int clientSocket);

void removePlayerFromGamersArray(int index);

void printPlayerArray();

int getFreePlayer();

//void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer);

bool attemptGameStart(int id);

int getSocketOfPlayer(int id);

void setTurnOfPlayer(int indexOfPlayer, int turn, int game);

void unSetTurnOfPlayer(int indexOfPlayer);

bool bothPlayersHaveTurn(int indexOfGame);

void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer);

int getTurnOfPlayer(int indexOfPlayer);

int getGameOfPlayer(int indexOfPlayer);

void printPlayerArray2();

void printGamesArray();

int getIdOfOpponent(int game, int id);

int getWhoTurnedSooner(int game);

#endif //SERVER_GAMEOBJECTS_H
