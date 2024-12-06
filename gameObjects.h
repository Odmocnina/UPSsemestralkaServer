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
};

void inicilazePlayerArray();


int addPlayerToGamersArray(char *name, int clientSocket);

void removePlayerFromGamersArray(int index);

void printPlayerArray();

int getFreePlayer();

//void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer);

bool attemptGameStart(int id);

int getSocketOfPlayer(int indexOfGame, bool who);

void setTurnOfPlayer(int indexOfPlayer, int turn);

bool bothPlayersHaveTurn(int indexOfGame);

void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer);

int getTurnOfPlayer(int indexOfPlayer);

int getGameOfPlayer(int indexOfPlayer);

void printPlayerArray2();

bool getWhoIsPlayer(int id);

#endif //SERVER_GAMEOBJECTS_H
