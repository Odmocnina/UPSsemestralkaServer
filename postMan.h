//
// Created by Michael on 18.11.2024.
//

#ifndef SERVER_POSTMAN_H
#define SERVER_POSTMAN_H

void *clientHandler(void *args);
int sendMessage(int socket, char *message, int length);
int sendPing(int socket);
void *pingHandler();

#endif //SERVER_POSTMAN_H
