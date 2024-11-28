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
#include <fcntl.h>

// Struktura argumentů pro vlákno
struct threadArgs {
    int clientSocket;
};

void resetNumber(int *number, int resetNumber) {
    if (number == NULL) {
        return;
    }
    *number = resetNumber;
}

// Obsluha klienta ve vláknu
void *clientHandler(void *args) {
    struct threadArgs *targs = (struct threadArgs *)args;
    int clientSocket = targs->clientSocket;
    free(targs);

    char bufferForMessage[MAX_SIZE_OF_MESSAGE];
    char fullMessage[MAX_SIZE_OF_MESSAGE] = {0};
    char bufferForSendBackMessage[MAX_SIZE_OF_MESSAGE] = {0};
    char signature[LENGTH_OF_MESSAGE_SIGNATURE + 1] = {0};
    int received = 0;
    int returnValue;
    int id;

    // Přijetí hlavičky (LENGTH_OF_MESSAGE_SIGNATURE)

    bool firstIteration = true;

    do {
        while (received < LENGTH_OF_MESSAGE_SIGNATURE) {
            printf("v prvnim while ");
            memset(bufferForMessage, 0, sizeof(bufferForMessage));
            returnValue = recv(clientSocket, bufferForMessage, LENGTH_OF_MESSAGE_SIGNATURE - received, 0);
            printf(bufferForMessage);
            if (returnValue > 0) {
                printf(" davam");
                memcpy(signature + received, bufferForMessage, returnValue);
                received = received + returnValue;
            } else if (returnValue == 0) {
                printf("Klient uzavřel spojení\n");
                close(clientSocket);
                return NULL;
            } else {
                printf("Chyba při čtení dat");
                close(clientSocket);
                return NULL;
            }
            printf("recived: %d\n", received);
        }
        printf("Přijatá signatura: %s\n", signature);

        strncpy(fullMessage, signature, LENGTH_OF_MESSAGE_SIGNATURE);

        // Přijetí zbytku zprávy, dokud nenarazí na '\n'
        received = LENGTH_OF_MESSAGE_SIGNATURE;
        bool foundNewline = false;
        while (!foundNewline) {
            memset(bufferForMessage, 0, sizeof(bufferForMessage));
            returnValue = recv(clientSocket, bufferForMessage + received, 1, 0); // de pres jedn znak ale upravit pak ze plus do toho bufferForMeessage
            if (returnValue > 0) {
                //for (int i = 0; i < returnValue; i = i + 1) {
                    if (bufferForMessage[received] == '\n') {
                        foundNewline = true;
                        //break;
                    }
                    fullMessage[received] = bufferForMessage[received];
                    received = received + 1;
                //}
            } else if (returnValue == 0) {
                printf("Klient uzavřel spojení\n");
                close(clientSocket);
                return NULL;
            } else {
                printf("Chyba při čtení dat");
                close(clientSocket);
                return NULL;
            }
        }
        fullMessage[received] = '\0';
        printf("Přijatá zpráva: %s\n", fullMessage);

        // Zpracování zprávy
        int messageOk = handleMessage(fullMessage, bufferForSendBackMessage);
        if (messageOk != FAILURE_VALUE) {
            printf("Odesílám klientovi: %s\n", bufferForSendBackMessage);
            returnValue = send(clientSocket, bufferForSendBackMessage, strlen(bufferForSendBackMessage), 0);
            if (returnValue < 0) {
                printf("Chyba při odesílání zprávy");
            }
            if (strstr(bufferForSendBackMessage, "login") != NULL) {
                int game = attemptGameStart(bufferForSendBackMessage);
                if (game != FAILURE_VALUE) {
                    returnValue = send(clientSocket, "Mess:gameBegin:\n", 15, 0);
                }
            }
            if (strstr(bufferForSendBackMessage, "logout") != NULL) {
                break;
            }
        }
        resetNumber(&received, -2); //tady je -2 protoze pokud je zpraovavana dalsi zpravat, tak to
        // jde do toho prvniho cyklu a nejak to prej precte 2veci, i kdyz je to neprcte
        memset(fullMessage, 0, sizeof(fullMessage));
        memset(bufferForSendBackMessage, 0, sizeof(bufferForSendBackMessage));
    } while (returnValue > 0);

    close(clientSocket);
    printf("Klient odpojen\n");
    return NULL;
}
