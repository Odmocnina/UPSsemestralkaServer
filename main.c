#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>

#include "gameObjects.h"
#include "postMan.h"




// Struktura argumentu pro vlákno
/**struktura pro agrumenty vlakna (socket)**/
struct threadArgs {
    int clientSocket;
};

/**
 *
 * Funkce co kotnroluje jestli je port v spravnem rozmenzi pro port
 *
 * @param pozt, ktery chceme zkontrolovat
 **/
bool isPortWithinBounds(int port) {
    return port >= 0 && port <= 65535;
}

/**
 * hlavni funkce programu. pokud se pripoji novi kleint je polsan do metody handleclient, je zde zalozeno vlakno
 * kontrolu pripojeni hracu, jestli pinguji
 *
 *
 * @param argc pocet argumentu prikazove radky
 * @param args argumentz prikzove radkz
 **/
int main(int argc, char* args[]) {

    char *endptr;
    long int port;

    if (argc == (2 + 1)) {

        port = strtol(args[2], &endptr, 10);

        if (errno == ERANGE) {
            printf("Spatny port\n");
            return FAILURE_VALUE;
        } else if (*endptr != '\0') {
            printf("Spatny port\n");
            return FAILURE_VALUE;
        } else if (!isPortWithinBounds(port)) {
            printf("Spatny port, neni v rozmezi pro port (0 - 65535)\n");
            return FAILURE_VALUE;
        }

        int serverSocket = 0;
        int clientSocket = 0;
        int returnValue = 0;

        int lenAdr;
        struct sockaddr_in maAddress, peerAddress;

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Nastaveni SO_REUSEADDR a SO_REUSEPORT
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("Chyba pri nastavováni SO_REUSEADDR");
            close(serverSocket);
            return -1;
        }

        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            perror("Chyba pri nastavováni SO_REUSEPORT");
            close(serverSocket);
            return -1;
        }

        memset(&maAddress, 0, sizeof(struct sockaddr_in));


        maAddress.sin_family = AF_INET;
        maAddress.sin_port = htons(port);
        //maAddress.sin_addr.s_addr = INADDR_ANY;
        inet_pton(AF_INET, args[1], &maAddress.sin_addr);

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
            printf("Chyba pri vytváreni vlákna pingHandler\n");
            return -1;
        }
        pthread_detach(pingThread);

        bool serverIsRunning = true;

        while (serverIsRunning == true) {
            clientSocket = accept(serverSocket, (struct sockaddr *) &peerAddress, &lenAdr);
            if (clientSocket > 0) {            //novy klient nalezen
                printf("Nove spojeni\n");

                // Vytvoreni vlákna pro obsluhu klienta
                pthread_t threadId;
                struct threadArgs *args = malloc(sizeof(struct threadArgs));
                args->clientSocket = clientSocket;

                if (pthread_create(&threadId, NULL, clientHandler, args) != 0) { //vytvoreni vlakna pro noveho klient
                    printf("Chyba pri vytvareni vlakna\n");
                    free(args);
                    close(clientSocket);
                }

                // Volitelně odpojit vlákno
                pthread_detach(threadId);
            } else {
                printf("Chyba pri prijeti spojeni\n");
            }
        }
    } else {
        printf("Spatny pocet argumentu\n");
    }

    return 0;
}