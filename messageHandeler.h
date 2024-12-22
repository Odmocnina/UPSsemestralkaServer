//
// Created by Michael on 13.11.2024.
//

#ifndef SERVER_MESSAGEHANDELER_H
#define SERVER_MESSAGEHANDELER_H

int handleMessage(char *message, char *sendBackMessage, int clientSocket, int *id);

int handleMessageComplicated(int clientSocket, int id, int messageType, int *returnValue);

int getTurnFormMessage(char *message);

#endif //SERVER_MESSAGEHANDELER_H
