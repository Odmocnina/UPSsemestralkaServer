#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "messageHandeler.h"
#include "gameObjects.h"
#include "postMan.h"




// Struktura argumentů pro vlákno
struct threadArgs {
    int clientSocket;
};

/*// Obsluha klienta ve vláknu
void *clientHandler(void *args) {
    struct threadArgs *targs = (struct threadArgs *)args;
    int clientSocket = targs->clientSocket;
    free(targs);

    char bufferForMessage[MAX_SIZE_OF_MESSAGE];
    int returnValue;

    //addPlayerToGamersArray("Untiteled");
    printPlayerArray();

    do {
        memset(bufferForMessage, 0, sizeof(bufferForMessage));
        returnValue = recv(clientSocket, bufferForMessage, sizeof(bufferForMessage) - 1, 0);
        if (returnValue > 0) {
            printf("Přijato: %s\n", bufferForMessage);
            handleMessage(bufferForMessage);
            //strncpy(bufferForMessage, "jasně vole\n", sizeof(bufferForMessage) - 1);
            strncpy(bufferForMessage, "Mess:\n", sizeof(bufferForMessage) - 1);
            send(clientSocket, bufferForMessage, strlen(bufferForMessage), 0);
        }
    } while (returnValue > 0);

    close(clientSocket);
    printf("Klient odpojen\n");
    return NULL;
}*/


int main() {

    int serverSocket = 0;
    int clientSocket = 0;
    int returnValue = 0;

    int lenAdr;
    struct sockaddr_in maAddress, peerAddress;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&maAddress, 0, sizeof(struct sockaddr_in));

    maAddress.sin_family = AF_INET;
    maAddress.sin_port = htons(PORT);
    maAddress.sin_addr.s_addr = INADDR_ANY;

    returnValue = bind(serverSocket, (struct sockaddr *) &maAddress, \
	sizeof(struct sockaddr_in));

    if (returnValue == 0)
        printf("Bind - OK\n");
    else {
        printf("Bind - ERR\n");
        return -1;
    }

    inicilazePlayerArray();

    listen(serverSocket, 5);

    bool serverIsRunning = true;

    while (serverIsRunning == true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&peerAddress, &lenAdr);
        if (clientSocket > 0) {
            printf("Nové spojení\n");

            // Vytvoření vlákna pro obsluhu klienta
            pthread_t threadId;
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            args->clientSocket = clientSocket;

            if (pthread_create(&threadId, NULL, clientHandler, args) != 0) {
                perror("Chyba pri vytvareni vlakna");
                free(args);
                close(clientSocket);
            }

            // Volitelně odpojit vlákno
            pthread_detach(threadId);
        } else {
            perror("Chyba pri prijeti spojeni");
        }
    }

    return 0;
}