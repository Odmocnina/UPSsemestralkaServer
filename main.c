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

int main() {

    int serverSocket = 0;
    int clientSocket = 0;
    int returnValue = 0;

    int lenAdr;
    struct sockaddr_in maAddress, peerAddress;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Nastavení SO_REUSEADDR a SO_REUSEPORT
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Chyba při nastavování SO_REUSEADDR");
        close(serverSocket);
        return -1;
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Chyba při nastavování SO_REUSEPORT");
        close(serverSocket);
        return -1;
    }

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

    //pthread_t pingThread;
    //pthread_create(&pingThread, NULL, pingHandler, NULL);

    pthread_t pingThread;
    if (pthread_create(&pingThread, NULL, checkPlayers, NULL) != 0) {
        printf("Chyba při vytváření vlákna pingHandler\n");
        return -1;
    }
    pthread_detach(pingThread);

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
                printf("Chyba pri vytvareni vlakna");
                free(args);
                close(clientSocket);
            }

            // Volitelně odpojit vlákno
            pthread_detach(threadId);
        } else {
            printf("Chyba pri prijeti spojeni");
        }
    }

    return 0;
}