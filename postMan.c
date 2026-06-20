//
// Created by Michael on 18.11.2024.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>

#include "messageHandeler.h"
#include "gameObjects.h"
#include "game.h"

/**pole booleanu co v sobe udrzuje kdo je pripojen a kdo ne, jes asi by to slo vyresit pres promenou v stuture player,
 * maybe, maybe, but uz mi mrda && tohle funguje = pouziju tohle, =========Z STAREHO RECONNECTU POZUSTATE===========
 * ........ ale nejde smazat
 **/
volatile bool connectionOfPlayers[MAX_NUMBER_OF_PLAYERS] = {true};
/**zamek pro pristup k ty prasarne o radek vis (vic vlaken k tomu pristupuje)**/
pthread_mutex_t lockPostMan = PTHREAD_MUTEX_INITIALIZER; //zamek pro pole konekci

/**Struktura pro argumenty v vlanku klienta**/
struct threadArgs {
    int clientSocket;
};

/**
 * Procedura na vyresetovani integeru
 *
 *
 * @param number ukazatel na cislo, ktere chceme vyrestovat
 * @param resetNumber cislo, na ktere chceme vyresetovat
 **/
void resetNumber(int *number, int resetNumber) {
    if (number == NULL) {
        return;
    }
    *number = resetNumber;
}

/**
 * Funkce na posilani zprav klientovi pres socket
 *
 *
 * @param socket socket kam chceme zpravu poslat
 * @param message char * ve kterem je zprava co chceme poslat
 * @param length delka zpravy co posilame
 * @return pocet bitu bo bylo poslano, pokud se nepodarilo zpravu poslat bude hodnota zaporna
 **/
int sendMessage(int socket, char *message, int length) {
    printf("Posilam: %s", message);
    int navrat = send(socket, message, length, 0);
    if (navrat < 0) {
        printf("Chyba pri odesilani zpravy\n");
    }
    return navrat;
}

int sendPing(int socket) {
    char pingMessage[11] = "Mess:ping:\n";
    int returnValue = sendMessage(socket, pingMessage, 11);
    if (returnValue < 0) {
        printf("Chyba pri odesilani ping zpravy\n");
        return FAILURE_VALUE;
    }
    return SUCCESS_VALUE;
}

/**
 * Funkce na nasteveni pozice v poli connectionOfPlayers, pred pristupem do pole je pouzit mutex aby nedochazelo
 * k soubehu
 *
 * @param id index v poli connectionOfPlayers
 * @param connection hodnota na kterou chceme nastavit v pozici v poli connectionOfPlayers
 * @return SUCCESS_VALUE
 **/
int setConnection(int id, bool connection) {
    //printf("setuju connectction pro id %d na %d\n", id, connection);
    pthread_mutex_lock(&lockPostMan);
    connectionOfPlayers[id] = connection;
    pthread_mutex_unlock(&lockPostMan);
    return SUCCESS_VALUE;
}

/**
 * Metoda na kontrolu jestli jou hraci pripojeni a pinguji, bezi v separatnim vlakne
 *
 *
 * @return void * aby mohlo bězet v pthread_create
 **/
void *checkPlayers() {
    bool checking = true;
    while (checking) {
        //printf("check players\n");
        for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
            checkPlayer(i);
        }
        sleep(1);
    }
}

/**
 * Funkce na zpracovanavani zprav od uzivatele ktery se pripojil, tato metoda je metda, ktrera se da do vlakna, kdyz se
 * klient pripoji
 *
 *
 * @param args ukazatel na strukturu s argumenty pro vlakno
 * @return NULL po prvedeni cele funkce
 **/
void *clientHandler(void *args) {

    struct threadArgs *targs = (struct threadArgs *)args;
    int clientSocket = targs->clientSocket;
    free(targs);

    struct timeval timeout;  //nastveni ze pokud 5 sekund recv nic neprijme tak to vyhodi specifickou hodnutu
    timeout.tv_sec = 5;  // casovy limit 5 sekund
    timeout.tv_usec = 0;

    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    char bufferForMessage[MAX_SIZE_OF_MESSAGE]; //buffer pro zpravu
    char fullMessage[MAX_SIZE_OF_MESSAGE] = {0}; //prijata zprava
    char bufferForSendBackMessage[MAX_SIZE_OF_MESSAGE] = {0}; //buffer pro zpravu zpet
    int received = 0;   //hodnta kolik bytu bylo prijato
    int returnValue;
    int id = ID_NOT_GIVEN_YET;     //id hrace/klienta
    int i = 0;
    bool toStart = false; //podminka ktera urcuje jestli se ma podivat na kdyby chtel hlavni cyklus stopnut
    bool disconnect = false;
    bool connectionState;

    do {
        toStart = false;
        if (id != ID_NOT_GIVEN_YET) {  //pozustatky stareho reconnectu, ale smazat to nejde, cely se to pak domrda
            pthread_mutex_lock(&lockPostMan);
            connectionState = connectionOfPlayers[id];
            pthread_mutex_unlock(&lockPostMan);
            if (connectionState == false) {
                disconnect = true;
                break;
            }
        }
        //received = 0;//LENGTH_OF_MESSAGE_SIGNATURE;
        bool foundNewline = false;
        while (!foundNewline) {
            memset(bufferForMessage, 0, sizeof(bufferForMessage));
            returnValue = recv(clientSocket, bufferForMessage + received, 1, 0); // de pres jedn znak ale upravit pak ze plus do toho bufferForMeessage
            if (returnValue > 0) {
                //printf("%c", bufferForMessage[received]);
                if (bufferForMessage[received] == '\n') {
                    foundNewline = true;
                }
                fullMessage[received] = bufferForMessage[received];
                received = received + 1;
            } else if (returnValue == 0) {
                printf("Klient uzavrel spojeni\n");
                if (getStateOfPlayer(id) == IN_GAME_VALUE || getStateOfPlayer(id) == IN_GAME_WAITING_VALUE) {
                    //printf("v if vnitrni");
                    infromOpponent(id, FAILURE_VALUE);
                }
                disconnectPlayer(id);
                close(clientSocket);
                return NULL;
            }  else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout, znovu zkontrolujeme stav
                //printf("zádná data nebyla prijata za 5 sekund\n");
                toStart = true;
                break;
                //continue;
            } else {
                printf("Chyba pri cteni dat");
                if (getStateOfPlayer(id) == IN_GAME_VALUE || getStateOfPlayer(id) == IN_GAME_WAITING_VALUE) {
                    //printf("v if vnitrni");
                    infromOpponent(id, FAILURE_VALUE);
                }
                disconnectPlayer(id);
                close(clientSocket);
                return NULL;
            }
        }
        if (toStart) { //tohle je docela hnusny ale it gets the job done
            continue;  //system tohdle je tak ze pokud recv nic nedostne zadnou zpravu tak
        }              //tak aby se to nezaseklo tak to pres boolean zaleze sem pak to posle pres kontinue na podminku
                       //kde se podiva jestli se ma podkracovat, cos bude vzdy a pokd se uzivatel odpojil tak prvni if
        fullMessage[received] = '\0';

        // Zpracováni zprávy
        int messageType = handleMessage(fullMessage, bufferForSendBackMessage, clientSocket, &id);
        if (messageType != FAILURE_VALUE) { //jestli je zprava OK tak dal zpravu zpracuj
            printf("Prijatá zpráva: %s", fullMessage);
            returnValue = sendMessage(clientSocket, bufferForSendBackMessage,
                                      strlen(bufferForSendBackMessage));

            if (returnValue < 0) {
                printf("Chyba pri odesiláni zprávy");
            }
            if (messageType == LOGIN_VALUE) {
                pthread_mutex_lock(&lockPostMan);
                connectionOfPlayers[id] = true;
                pthread_mutex_unlock(&lockPostMan);
            }
            if (messageType == PING_VALUE) {
                setNumberOfPongs(id, getNumberOfPongs(id) + 1);
            }
            if (messageType == NAME_ALREADY_USED) {
                break;
            }
            if (messageType == LOGOUT_VALUE) {
                disconnectPlayer(id);
                disconnect = true;
                break;
            } else {
                handleMessageComplicated(clientSocket, id, messageType, &returnValue);
            }
        } else {
            printf("Prijatá zpráva: nevalidni\n");
            sendMessage(clientSocket, "Mess:invalidMessage:\n", 20);
            if (getStateOfPlayer(id) == IN_GAME_VALUE || getStateOfPlayer(id) == IN_GAME_WAITING_VALUE) {
                infromOpponent(id, FAILURE_VALUE);
                removeGame(id);
            }
            disconnectPlayer(id);
            disconnect = true;
            break;
        }
        resetNumber(&received, 0);
        memset(fullMessage, 0, sizeof(fullMessage));
        memset(bufferForSendBackMessage, 0, sizeof(bufferForSendBackMessage));
        i = i + 1;
        pthread_mutex_lock(&lockPostMan);
        connectionState = connectionOfPlayers[id];
        pthread_mutex_unlock(&lockPostMan);
        if (id == ID_NOT_GIVEN_YET) {
            connectionState = true;
        }
    } while ((toStart || returnValue > 0) && connectionState);

    disconnect = false;
    if (disconnect) { //Drive se odpojovalo tady, nyni je to primo v whilu pokud se clovek odpoji/domrda pripojeni
        if (getStateOfPlayer(id) == IN_GAME_VALUE || getStateOfPlayer(id) == IN_GAME_WAITING_VALUE) {
            infromOpponent(id, FAILURE_VALUE);
            removeGame(id);
        }
        disconnectPlayer(id);
    }
    close(clientSocket);
    printf("Klient odpojen\n");
    setConnection(id, true);
    return NULL;
}