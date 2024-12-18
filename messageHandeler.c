//
// Created by Michael on 13.11.2024.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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


int handleMessage(char *message, char *sendBackMessage, int clientSocket) {
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
                int id = handleLogin(fullMessageForInspection, clientSocket);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "login", id);
                }
                navrat = id;
            } else if (strcmp(typeOfMessage, "logout") == STRINGS_ARE_SAME) {
                int id = handlelogout(fullMessageForInspection);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "logout", id);
                }
            } else if (strcmp(typeOfMessage, "turn") == STRINGS_ARE_SAME) {
                int turn = handleTurn(fullMessageForInspection);
                if (turn != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "turn", turn);
                }
                navrat = turn;
            } else if (strcmp(typeOfMessage, "game") == STRINGS_ARE_SAME) {
                makeMessage(sendBackMessage, "game", -1);
                navrat = SUCCESS_VALUE;
            }
        }
        typeOfMessage[j] = fullMessageForInspection[i];
        i = i + 1;
        j = j + 1;
    }

    return navrat;
}
