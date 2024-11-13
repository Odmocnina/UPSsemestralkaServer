//
// Created by Michael on 12.11.2024.
//

#include "gameObjects.h"
#include "constants.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

struct player players[MAX_NUMBER_OF_PLAYERS];

void inicilazePlayerArray() {
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        players[i].state = FREE_POSITION;
    }
}

void addPlayerToGamersArray(char *name) {
    int i = 0;
    bool found = false;
    while (!found && i < MAX_NUMBER_OF_PLAYERS) {
        if (players[i].state == FREE_POSITION) {
            strncpy(players[i].name, name, sizeof(players[i].name));
            players[i].name[MAX_NAME_LENGTH - 1] = '\0';
            players[i].state = IN_LOBBY_VALUE;
            players[i].index = i;
            found = true;
        }
        i = i + 1;
    }
}

void printPlayerArray() {
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
    }
    printf("\n");
}
