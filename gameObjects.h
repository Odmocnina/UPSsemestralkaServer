//
// Created by Michael on 12.11.2024.
//

#ifndef SERVER_GAMEOBJECTS_H
#define SERVER_GAMEOBJECTS_H

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

void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer);

int attemptGameStart(char *message);

int getSocketOfPlayer(int indexOfPlayer);

#endif //SERVER_GAMEOBJECTS_H
