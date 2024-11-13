//
// Created by Michael on 13.11.2024.
//

#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "messageHandeler.h"
#include "gameObjects.h"

int handleLogin(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    printf("prihlasen: %s\n", token);
    addPlayerToGamersArray(token);
}

int handleMessage(char *message) {
    printf("Prijata zprava: %s\n", message);
    if (strncmp(message, "Mess", LENGTH_OF_MESSAGE_SIGNATURE) != 0) {
        printf("Message is not valid\n");
        return MESSAGE_NOT_VALID;
    }

    printf("Message is valid\n");
    int i = START_OF_MESSAGE;
    int j = 0;
    char typeOfMessage[MAXIMAL_LENGHT_OF_MESSAGE_TYPE];
    while (message[i] != '\n') {
        if (message[i] == ':') {  // Pokud narazíme na středník, ukončíme parsování
            typeOfMessage[j] = '\0';  // Ukončíme slov
            if (strcmp(typeOfMessage, "login") == 0) {
                printf("login\n");
                handleLogin(message);
            }
        }
        typeOfMessage[j] = message[i];
        i = i + 1;
        j = j + 1;
    }

    printPlayerArray();

    return SUCCESS_VALUE;
}
