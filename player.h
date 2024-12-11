//
// Created by Michael on 13.11.2024.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "constants.h"

struct player {
    char name[MAX_NAME_LENGTH];
    int state;
    int index; //funguje zaroven jako id
    int clientSocket;
    //int time;
    int turn;
    int game;
};

#endif //SERVER_PLAYER_H
