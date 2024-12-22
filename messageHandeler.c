//
// Created by Michael on 13.11.2024.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "constants.h"
#include "messageHandeler.h"
#include "gameObjects.h"
#include "game.h"
#include "postMan.h"

int handlelogout(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    removePlayerFromGamersArray(atoi(token));
    return atoi(token);
}

void makeMessage(char *buffer, char *type, int id) {
    //char idInChar = id;  //muj genius je tak velky, ze ma vlastni gravitacni pole
    if (strcmp(type, "login") == STRINGS_ARE_SAME) {
        sprintf(buffer, "Mess:%s:%d:\n", type, id);
    } else {
        sprintf(buffer, "Mess:%s:OK:\n", type);
    }
    //sprintf(buffer, "Mess:%s:%d:\n", type, id);
    //printf(buffer);
    //sprintf();
}

int handleLogin(char *message, int clientSocket) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    int navrat = addPlayerToGamersArray(token, clientSocket);
    if (navrat == FAILURE_VALUE) {
        printf("Moc hracu\n");
    } else {
        printf("Hrac pridan\n");
        printf("Prihlasen: %s\n", token);
    }
    //int gameStartAttempt = handleGameStart(navrat);

    return navrat;
}

int handleTurn(char *message, int id) {
    strtok(message, ":");
    strtok(NULL, ":");
    //int id = atoi(strtok(NULL, ":"));
    int turn = atoi(strtok(NULL, ":"));
    setTurnOfPlayer(id, turn);
    int navrat = SUCCESS_VALUE;
    return navrat;
}

//int getTurnFormMessage(char *message) {
//    return handleTurn(message);
//}

char *trimLeft(char *str) {
    // Kontrola prázdného vstupu
    if (str == NULL) {
        return NULL;
    }

    // Najdi první znak, který není bílý
    while (isspace((unsigned char)*str)) {
        str = str + 1;
    }

    return str; // Vrátí ukazatel na začátek "ořezaného" řetězce
}


int handleMessage(char *message, char *sendBackMessage, int clientSocket, int *id) {
    message = trimLeft(message);
    if (strncmp(message, "Mess:", LENGTH_OF_MESSAGE_SIGNATURE + 1) != 0) {
        printf("Zprava neni validni\n");
        return FAILURE_VALUE;
    }

    char fullMessageForInspection[strlen(message)];
    strcpy(fullMessageForInspection, message);

    printf("Zprava je validni\n");
    int i = START_OF_MESSAGE;
    int j = 0;
    int navrat = FAILURE_VALUE;
    char typeOfMessage[MAXIMAL_LENGHT_OF_MESSAGE_TYPE];
    while (fullMessageForInspection[i] != '\n') {
        if (fullMessageForInspection[i] == ':') {  // Pokud narazíme na středník, ukončíme parsování
            typeOfMessage[j] = '\0';  // Ukončíme slov
            if (strcmp(typeOfMessage, "login") == STRINGS_ARE_SAME) {
                *id = handleLogin(fullMessageForInspection, clientSocket);
                if (*id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "login", *id);
                }
                navrat = LOGIN_VALUE;
            } else if (strcmp(typeOfMessage, "logout") == STRINGS_ARE_SAME) {
                int id = handlelogout(fullMessageForInspection);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "logout", id);
                }
                navrat = LOGOUT_VALUE;
            } else if (strcmp(typeOfMessage, "turn") == STRINGS_ARE_SAME) {
                int turn = handleTurn(fullMessageForInspection, *id);
                if (turn != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "turn", turn);
                }
                navrat = TURN_VALUE;
            } else if (strcmp(typeOfMessage, "game") == STRINGS_ARE_SAME) {
                makeMessage(sendBackMessage, "game", -1);
                navrat = GAME_VALUE;
            }
        }
        typeOfMessage[j] = fullMessageForInspection[i];
        i = i + 1;
        j = j + 1;
    }

    printPlayerArray2();

    return navrat;
}

int handleMessageComplicated(int clientSocket, int id, int messageType, int *returnValue) {
    int game;
    int opponetId;
    //cislo = 0;
    if (messageType == LOGIN_VALUE) {
        //id = messageOk;
        bool isGame = attemptGameStart(id);
        game = getGameOfPlayer(id);
        if (isGame != false) {
            char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
            opponetId = getIdOfOpponent(game, id);
            *returnValue = sendMessage(clientSocket, gameBegin, strlen(gameBegin));
            sendMessage(getSocketOfPlayer(opponetId), gameBegin, strlen(gameBegin));
        }
    }
    if (messageType == TURN_VALUE) {
        //setTurnOfPlayer(id, messageOk, game);
        game = getGameOfPlayer(id);
        opponetId = getIdOfOpponent(game, id);
        if (bothPlayersHaveTurn(game)) {
            char messageForFirstPlayer[MAX_SIZE_OF_MESSAGE];
            char messageForSecondPlayer[MAX_SIZE_OF_MESSAGE];
            //int whoSooner = getWhoTurnedSooner(game);
            handleGame(game, messageForFirstPlayer, messageForSecondPlayer);
            if (isFirstPlayer(id, game)) {
                sendMessage(clientSocket, messageForFirstPlayer, strlen(messageForFirstPlayer));
                sendMessage(getSocketOfPlayer(opponetId), messageForSecondPlayer, strlen(messageForSecondPlayer));
            } else {
                sendMessage(getSocketOfPlayer(opponetId), messageForFirstPlayer,
                            strlen(messageForFirstPlayer));
                sendMessage(clientSocket, messageForSecondPlayer,
                            strlen(messageForSecondPlayer));
            }
            char bothPlayerTurn[LENGTH_OF_BOTHPLAYERS_TURED_MESSAGE] = "Mess:bothPlayerTurn:\n";
            sendMessage(clientSocket, bothPlayerTurn, strlen(bothPlayerTurn));
            sendMessage(getSocketOfPlayer(opponetId), bothPlayerTurn, strlen(bothPlayerTurn));
            unSetTurnOfPlayer(id);
            unSetTurnOfPlayer(opponetId);
            //unSetWhoTurnedFirst(game);
        }
    }
    if (messageType == GAME_VALUE) {
        game = getGameOfPlayer(id);
        unsetGame(game);
        freePlayer(id);
        game = FAILURE_VALUE;
        opponetId = FAILURE_VALUE;
        bool isGame = attemptGameStart(id);
        game = getGameOfPlayer(id);
        if (isGame != false) {
            char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
            opponetId = getIdOfOpponent(game, id);
            //printf("delka zpravy: %d" , strlen(gameBegin));
            *returnValue = sendMessage(clientSocket, gameBegin, LENGHT_OF_START_GAME_MESSAGE);
            sendMessage(getSocketOfPlayer(opponetId), gameBegin, LENGHT_OF_START_GAME_MESSAGE);
        }
    }

    return SUCCESS_VALUE;
}
