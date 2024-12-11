//
// Created by Michael on 13.11.2024.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "messageHandeler.h"
#include "gameObjects.h"

int handlelogout(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    removePlayerFromGamersArray(atoi(token));
    return atoi(token);
}

void makeMessage(char *buffer, char *type, int id) {
    //char idInChar = id;  //muj genius je tak velky, ze ma vlastni gravitacni pole
    sprintf(buffer, "Mess:%s:%d:\n", type, id);
    printf(buffer);
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

    printf("Pred navratem");
    return navrat;
}

int handleTurn(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    //int id = atoi(strtok(NULL, ":"));
    int turn = atoi(strtok(NULL, ":"));
    int navrat = FAILURE_VALUE;
    navrat = turn;
    return navrat;
}

int getTurnFormMessage(char *message) {
    return handleTurn(message);
}

int handleMessage(char *message, char *sendBackMessage, int clientSocket) {
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
                printf("Login\n");
                int id = handleLogin(fullMessageForInspection, clientSocket);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "login", id);
                }
                printf("Po vytvoreni zpravy %d\n", id);
                navrat = id;
            } else if (strcmp(typeOfMessage, "logout") == STRINGS_ARE_SAME) {
                printf("logout\n");
                int id = handlelogout(fullMessageForInspection);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "logout", id);
                }
            } else if (strcmp(typeOfMessage, "turn") == STRINGS_ARE_SAME) {
                printf("turn\n");
                int turn = handleTurn(fullMessageForInspection);
                if (turn != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "turn", turn);
                }
                navrat = turn;
            } else if (strcmp(typeOfMessage, "ready") == STRINGS_ARE_SAME) {
                printf("ready\n");
                unSetTurnOfPlayer(clientSocket);//soket zde se bude predavat jako id
            }
        }
        typeOfMessage[j] = fullMessageForInspection[i];
        i = i + 1;
        j = j + 1;
    }

    return navrat;
}
