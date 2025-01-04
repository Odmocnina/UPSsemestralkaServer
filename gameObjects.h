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
    int numberOfPlayedRounds;
    int firstPlayerScore;
    int secondPlayerScore;
    int numberOfStalemates;
    bool gameHalted; // pro krizkovy reconnect
};

void inicilazePlayerArray();


int addPlayerToGamersArray(char *name, int clientSocket);

void removePlayerFromGamersArray(int index);

void printPlayerArray();

int getFreePlayer();

//void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer);

bool attemptGameStart(int id);

int getSocketOfPlayer(int id);

void setTurnOfPlayer(int indexOfPlayer, int turn);

void unSetTurnOfPlayer(int indexOfPlayer);

bool bothPlayersHaveTurn(int indexOfGame);

void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer);

int getTurnOfPlayer(int indexOfPlayer);

int getGameOfPlayer(int indexOfPlayer);

void printPlayerArray2();

void printGamesArray();

int getIdOfOpponent(int game, int id);

void updateGameScore(int game, int player1Gain, int player2Gain, int stalemateGain);

int getScoreOfFirstPlayer(int game);

int getScoreOfSecondPlayer(int game);

int getNumberOfRounds(int game);

bool isFirstPlayer(int id, int game);

int unsetGame(int game);

int freePlayer(int id);

int getStalemates(int game);

int sendPingToAllPlayers();

int setPingOfPlayer(int id);

bool checkName(char *name);

int getNumberOfPings(int id);

int getNumberOfPongs(int id);

int setNumberOfPings(int id, int numberOfPings);

int setNumberOfPongs(int id, int numberOfPongs);

int checkPlayer(int id);

int setTimeSinceLastPing(int id, long time);

int disconnectPlayer(int id);

bool getIsConnectedOrTryingToConnect(int id);

bool getConnection2(int id);

int setConnection2(int id, bool connection);

int getStateOfPlayer(int id);

int setStateOfPlayer(int id, int state);

#endif //SERVER_GAMEOBJECTS_H
