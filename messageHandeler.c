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

    return navrat;
}

int handleMessage(char *message, char *sendBackMessage, int clientSocket) {

    printf("Prijata zprava: %s", message);
    if (strncmp(message, "Mess:", LENGTH_OF_MESSAGE_SIGNATURE + 1) != 0) {
        printf("Zprava neni validni\n");
        return FAILURE_VALUE;
    }

    printf("Zprava je validni\n");
    int i = START_OF_MESSAGE;
    int j = 0;
    char typeOfMessage[MAXIMAL_LENGHT_OF_MESSAGE_TYPE];
    while (message[i] != '\n') {
        if (message[i] == ':') {  // Pokud narazíme na středník, ukončíme parsování
            typeOfMessage[j] = '\0';  // Ukončíme slov
            if (strcmp(typeOfMessage, "login") == STRINGS_ARE_SAME) {
                printf("Login\n");
                int id = handleLogin(message, clientSocket);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "login", id);
                }
            } else if (strcmp(typeOfMessage, "logout") == STRINGS_ARE_SAME) {
                printf("logout\n");
                int id = handlelogout(message);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "logout", id);
                }
            }
        }
        typeOfMessage[j] = message[i];
        i = i + 1;
        j = j + 1;
    }

    printPlayerArray();

    return SUCCESS_VALUE;
}
