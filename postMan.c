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

void *pingHandler(void *args) {
    int clientSocket = *(int *)args;
    free(args); // Pokud alokujete paměť pro předávání argumentů.

    const char *pingMessage = "ping\n";
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
        if (strcmp(pongResponse, "pong\n") != 0) {
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

    /*pthread_t pingThread;
    int *clientSocketCopy = malloc(sizeof(int));
    if (clientSocketCopy == NULL) {
        perror("Chyba při alokaci paměti");
        close(clientSocket);
        return NULL;
    }
    *clientSocketCopy = clientSocket;

    // Spuštění vlákna pro pingování
    if (pthread_create(&pingThread, NULL, pingHandler, clientSocketCopy) != 0) {
        perror("Nepodařilo se vytvořit vlákno pro pingování");
        close(clientSocket);
        free(clientSocketCopy);
        return NULL;
    }
    pthread_detach(pingThread); // Uvolní vlákno automaticky po ukončení.*/

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
        while (received < LENGTH_OF_MESSAGE_SIGNATURE) {
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
        printf("Přijatá zpráva: %s", fullMessage);

        // Zpracování zprávy
        int messageOk = handleMessage(fullMessage, bufferForSendBackMessage, clientSocket);
        if (messageOk != FAILURE_VALUE) {
            returnValue = send(clientSocket, bufferForSendBackMessage, strlen(bufferForSendBackMessage), 0);
            if (returnValue < 0) {
                printf("Chyba při odesílání zprávy");
            }
            if (strstr(fullMessage, "login") != NULL) {
                id = messageOk;
                bool isGame = attemptGameStart(id);
                game = getGameOfPlayer(id);
                if (isGame != false) {
                    char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
                    opponetId = getIdOfOpponent(game, id);
                    returnValue = send(clientSocket, gameBegin, strlen(gameBegin), 0);
                    send(getSocketOfPlayer(opponetId), gameBegin, strlen(gameBegin), 0);
                }
            }
            if (strstr(fullMessage, "turn") != NULL) {
                setTurnOfPlayer(id, messageOk, game);
                game = getGameOfPlayer(id);
                opponetId = getIdOfOpponent(game, id);
                if (bothPlayersHaveTurn(game)) {
                    char messageForFirstPlayer[MAX_SIZE_OF_MESSAGE];
                    char messageForSecondPlayer[MAX_SIZE_OF_MESSAGE];
                    //int whoSooner = getWhoTurnedSooner(game);
                    handleGame(game, messageForFirstPlayer, messageForSecondPlayer);
                    if (isFirstPlayer(id, game)) {
                        send(clientSocket, messageForFirstPlayer, strlen(messageForFirstPlayer), 0);
                        send(getSocketOfPlayer(opponetId), messageForSecondPlayer, strlen(messageForSecondPlayer), 0);
                    } else {
                        send(getSocketOfPlayer(opponetId), messageForFirstPlayer, strlen(messageForFirstPlayer), 0);
                        send(clientSocket, messageForSecondPlayer, strlen(messageForSecondPlayer), 0);
                    }
                    printf("posilam ze oba hraci hrali");
                    char bothPlayerTurn[LENGTH_OF_BOTHPLAYERS_TURED_MESSAGE] = "Mess:bothPlayerTurn:\n";
                    send(clientSocket, bothPlayerTurn, strlen(bothPlayerTurn), 0);
                    send(getSocketOfPlayer(opponetId), bothPlayerTurn, strlen(bothPlayerTurn), 0);
                    unSetTurnOfPlayer(id);
                    unSetTurnOfPlayer(opponetId);
                    //unSetWhoTurnedFirst(game);
                }
            }
            if (strstr(fullMessage, "game") != NULL) {
                unsetGame(game);
                freePlayer(id);
                game = FAILURE_VALUE;
                opponetId = FAILURE_VALUE;
                bool isGame = attemptGameStart(id);
                game = getGameOfPlayer(id);
                if (isGame != false) {
                    char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
                    opponetId = getIdOfOpponent(game, id);
                    //printf("delka zpravy: %d" , strlen(gameBegin));
                    returnValue = send(clientSocket, gameBegin, LENGHT_OF_START_GAME_MESSAGE, 0);
                    send(getSocketOfPlayer(opponetId), gameBegin, LENGHT_OF_START_GAME_MESSAGE, 0);
                }
            }
            if (strstr(fullMessage, "logout") != NULL) {
                break;
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
