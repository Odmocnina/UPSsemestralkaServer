//
// Created by Michael on 18.11.2024.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "messageHandeler.h"
#include "gameObjects.h"
#include <pthread.h>
#include <stdbool.h>

// Struktura argumentů pro vlákno
struct threadArgs {
    int clientSocket;
};

// Obsluha klienta ve vláknu
void *clientHandler(void *args) {
    struct threadArgs *targs = (struct threadArgs *)args;
    int clientSocket = targs->clientSocket;
    free(targs);

    char bufferForMessage[MAX_SIZE_OF_MESSAGE];
    char bufferForSendBackMessage[MAX_SIZE_OF_MESSAGE];
    int returnValue;

    //addPlayerToGamersArray("Untiteled");
    printPlayerArray();

    do {
        memset(bufferForMessage, 0, sizeof(bufferForMessage));
        returnValue = recv(clientSocket, bufferForMessage, sizeof(bufferForMessage) - 1, 0);
        if (returnValue > 0) {
            //printf("Prijato: %s", bufferForMessage);
            int messageOk = handleMessage(bufferForMessage, bufferForSendBackMessage);
            //strncpy(bufferForMessage, "jasně vole\n", sizeof(bufferForMessage) - 1);
            //strncpy(bufferForMessage, "Mess:\n", sizeof(bufferForMessage) - 1);
            if (messageOk != FAILURE_VALUE) {
                printf("Odesilam klientovi: %s", bufferForSendBackMessage);
                send(clientSocket, bufferForSendBackMessage, MAX_SIZE_OF_MESSAGE, 0);
            }
        }
    } while (returnValue > 0);

    printPlayerArray();

    close(clientSocket);
    printf("Klient odpojen\n");
    return NULL;
}
