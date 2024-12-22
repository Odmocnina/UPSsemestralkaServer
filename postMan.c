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
#include "game.h"
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>

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

int sendMessage(int socket, char *message, int length) {
    int navrat = send(socket, message, length, 0);
    if (navrat < 0) {
        printf("Chyba pri odesilani zpravy\n");
    }
    return navrat;
}

void *pingHandler(void *args) {
    int clientSocket = *(int *)args;
    free(args); // Pokud alokujete paměť pro předávání argumentů.

    const char *pingMessage = "Mess:břing:\n";
    char pongResponse[16] = {0};
    struct timeval timeout = {5, 0}; // Nastavení timeoutu na 5 sekund.
    int returnValue;

    while (1) {
        sleep(10); // Posíláme ping každých 10 sekund.
        printf("Posílám ping klientovi.\n");

        // Odeslání "ping"
        returnValue = send(clientSocket, pingMessage, strlen(pingMessage), 0);
        if (returnValue <= 0) {
            printf("Nepodařilo se odeslat ping. Odpojuji klienta.\n");
            close(clientSocket);
            pthread_exit(NULL);
        }

        // Nastavení timeoutu pro příjem odpovědi
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);

        // Příjem odpovědi "pong"
        returnValue = recv(clientSocket, pongResponse, sizeof(pongResponse) - 1, 0);
        if (returnValue <= 0) {
            printf("Klient neodpověděl na ping. Odpojuji klienta.\n");
            close(clientSocket);
            pthread_exit(NULL);
        }

        pongResponse[returnValue] = '\0'; // Ujistíme se, že máme validní řetězec.
        if (strcmp(pongResponse, "Mess:břong:\n") != 0) {
            printf("Neočekávaná odpověď od klienta: %s. Odpojuji klienta.\n", pongResponse);
            close(clientSocket);
            pthread_exit(NULL);
        }

        printf("Klient odpověděl na ping: %s\n", pongResponse);
    }

    return NULL;
}

int processMessage() {

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
    int opponetId;
    int game = FAILURE_VALUE;

    do {
        /*while (received < LENGTH_OF_MESSAGE_SIGNATURE) { //snad neni potreba
            memset(bufferForMessage, 0, sizeof(bufferForMessage));
            returnValue = recv(clientSocket, bufferForMessage, LENGTH_OF_MESSAGE_SIGNATURE - received, 0);
            if (returnValue > 0) {
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
        }

        if (strncmp(bufferForMessage, "Mess:", 5)) {
            printf(" %s ", bufferForMessage);
            printf("pruser");
        }

        strncpy(fullMessage, signature, LENGTH_OF_MESSAGE_SIGNATURE);*/

        // Přijetí zprávy, dokud nenarazí na '\n'
        received = 0;//LENGTH_OF_MESSAGE_SIGNATURE;
        bool foundNewline = false;
        while (!foundNewline) {
            memset(bufferForMessage, 0, sizeof(bufferForMessage));
            returnValue = recv(clientSocket, bufferForMessage + received, 1, 0); // de pres jedn znak ale upravit pak ze plus do toho bufferForMeessage
            if (returnValue > 0) {
                if (bufferForMessage[received] == '\n') {
                    foundNewline = true;
                }
                fullMessage[received] = bufferForMessage[received];
                received = received + 1;
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
        printf("Přijatá zpráva: %s", fullMessage);

        // Zpracování zprávy
        int messageType = handleMessage(fullMessage, bufferForSendBackMessage, clientSocket, &id);
        if (messageType != FAILURE_VALUE) {
            returnValue = sendMessage(clientSocket, bufferForSendBackMessage,
                                      strlen(bufferForSendBackMessage));

            if (returnValue < 0) {
                printf("Chyba při odesílání zprávy");
            }
            if (messageType == LOGOUT_VALUE) {
                break;
            } else {
                handleMessageComplicated(clientSocket, id, messageType, &returnValue);
            }
        }
        resetNumber(&received, 0);
        memset(fullMessage, 0, sizeof(fullMessage));
        memset(bufferForSendBackMessage, 0, sizeof(bufferForSendBackMessage));
    } while (returnValue > 0);

    close(clientSocket);
    printf("Klient odpojen\n");
    return NULL;
}
