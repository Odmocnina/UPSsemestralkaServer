//
// Created by Michael on 12.11.2024.
//

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "gameObjects.h"
#include "constants.h"
#include "postMan.h"

/**pole hracu, kazdy hrac zabira jednu pozici, id hrace je jeho index v poli**/
struct player players[MAX_NUMBER_OF_PLAYERS];

/**pole bezicich her**/
struct lobby runningGames[MAX_NUMBER_OF_PLAYERS / 2];

/**mutex pro zamknuti dat pristupovanych vice vlakny**/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * metoda pro inicializaci pole hracu, volana pri spusteni serveru
 **/
void inicilazePlayerArray() {
    pthread_mutex_lock(&lock);
    printf("Inicializuji pole hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        players[i].state = FREE_POSITION;

        players[i].index = FREE_POSITION;

        players[i].isConnectedOrTryingToConnect = true;
    }
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        runningGames[i].indexOfPlayer1 = FREE_POSITION;

        runningGames[i].indexOfPlayer2 = FREE_POSITION;

    }
    pthread_mutex_unlock(&lock);
}

/**
 * funkce pro pridani hrace do pole hracu
 *
 * @param name jemeno hrace
 * @param clientSocket socket pripojenho hrace
 * @return index v poli hracu, pokud se nepodrari pridat hrace je vraceno FAILURE_VALUE
 **/
int addPlayerToGamersArray(char *name, int clientSocket) {
    pthread_mutex_lock(&lock);
    int i = 0;
    bool found = false;
    int navrat = FAILURE_VALUE;
    while (!found && i < MAX_NUMBER_OF_PLAYERS) {
        if (players[i].state == FREE_POSITION) {
            strncpy(players[i].name, name, sizeof(players[i].name));
            players[i].name[MAX_NAME_LENGTH - 1] = '\0';
            players[i].state = WAITING_VALUE;
            players[i].index = i;
            players[i].clientSocket = clientSocket;
            players[i].numberOfPings = 0;
            players[i].numberOfPongs = 0;
            players[i].connectionGood = SUCCESS_VALUE;
            players[i].isConnectedOrTryingToConnect = true;
            players[i].timeSinceLastPing = getTimeInMili(); //kvuli tyhle mrdce to nefungovalo na skolnim zeleze
            navrat = i;
            found = true;
        }
        i = i + 1;
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * funkce pro ziskani volneho hrace
 *
 * @param indexOfConnectedPlayer index hrace ke kteremu hledame spoluhrace
 * @return index volneho hrace, pokud neni zadny hrac volny je vraceno FAILURE_VALUE
 **/
int getFreePlayer(int indexOfConnectedPlayer) {
    pthread_mutex_lock(&lock);
    int i = 0;
    int navrat = FAILURE_VALUE;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS)) {
        if (players[i].state == WAITING_VALUE && i != indexOfConnectedPlayer
                                    && players[i].connectionGood == SUCCESS_VALUE) {
            found = true;
            navrat = i;
        }
        i = i + 1;
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * podporgram pro odstraneni hrace z pole hracu
 *
 * @param index index hrace co ma byt smazan
 **/
void removePlayerFromGamersArray(int index) {
    pthread_mutex_lock(&lock);
    //players[index].state = FREE_POSITION;
    players[index].name[0] = '\0';
    players[index].index = FREE_POSITION;
    pthread_mutex_unlock(&lock);
}

/**
 * metoda pro vypsani pole hracu, pouzito pri debugu
 **/
void printPlayerArray() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

/**
 * metoda pro sofistikovanejsi vypsani pole hracu, pouzito pri debugu
 **/
void printPlayerArray2() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu hracu\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        printf("Jmeno: %s\n", players[i].name);
        printf("Stav: %d\n", players[i].state);
        printf("index: %d\n", players[i].index);
        printf("lobby: %d\n", players[i].game);
        printf("turn: %d\n", players[i].turn);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

/**
 * metoda pro vypsani pole her, pouzito pri debugu
 **/
void printGamesArray() {
    pthread_mutex_lock(&lock);
    printf("Zacatek vypisu her\n");
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        printf("id1: %d\n", runningGames[i].indexOfPlayer1);
        printf("id2: %d\n", runningGames[i].indexOfPlayer2);
        printf("kola: %d\n", runningGames[i].numberOfPlayedRounds);
        printf("1s: %d\n", runningGames[i].indexOfPlayer1);
        printf("2s: %d\n", runningGames[i].indexOfPlayer2);
    }
    printf("\n");
    pthread_mutex_unlock(&lock);
}

/**
 * funkce pro zjistieni skore prvniho hrace v hre
 *
 * @param game index hry
 **/
int getScoreOfFirstPlayer(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].firstPlayerScore;
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * funkce pro zjistieni skore druheho hrace v hre
 *
 * @param game index hry
 **/
int getScoreOfSecondPlayer(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].secondPlayerScore;
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * funkce pro zjistieni poctu kol co bylo v hre odehrano
 *
 * @param game index hry
 **/
int getNumberOfRounds(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].numberOfPlayedRounds;
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * podprogram pro updatovani herniho stavu hry
 *
 * @param game index hry
 * @param player1Gain zisk prvniho hrace
 * @param player2Gain zisk druheho hrace
 * @param stalemateGain pocet pridanych remiz
 **/
void updateGameScore(int game, int player1Gain, int player2Gain, int stalemateGain) {
    pthread_mutex_lock(&lock);
    runningGames[game].firstPlayerScore = runningGames[game].firstPlayerScore + player1Gain;
    runningGames[game].secondPlayerScore = runningGames[game].secondPlayerScore + player2Gain;
    runningGames[game].numberOfStalemates = runningGames[game].numberOfStalemates + stalemateGain;
    runningGames[game].numberOfPlayedRounds = runningGames[game].numberOfPlayedRounds + 1;
    pthread_mutex_unlock(&lock);
}

/**
 * vytvori novou hru
 *
 * @param indexOfFirstPlayer index prvniho hrace
 * @param indexOfSecondPlayer index druheho hrace
 * @param game int ukazatel do ktereho je dan index nove vytvorene hry
 **/
void addNewRunningGame(int indexFirstPlayer, int indexSecondPlayer, int *game) {
    pthread_mutex_lock(&lock);
    int i = 0;
    bool found = false;
    while (!found && i < (MAX_NUMBER_OF_PLAYERS / 2)) {
        if (runningGames[i].indexOfPlayer1 == FREE_POSITION) {
            found = true;
            runningGames[i].indexOfPlayer1 = indexFirstPlayer;
            runningGames[i].indexOfPlayer2 = indexSecondPlayer;
            runningGames[i].numberOfPlayedRounds = 0;
            runningGames[i].firstPlayerScore = 0;
            runningGames[i].secondPlayerScore = 0;
            runningGames[i].numberOfStalemates = 0;
            runningGames[i].gameHalted = false;
            players[indexFirstPlayer].state = IN_GAME_VALUE;
            players[indexSecondPlayer].state = IN_GAME_VALUE;
            players[indexFirstPlayer].turn = TURN_NOT_PICKED_YET;
            players[indexSecondPlayer].turn = TURN_NOT_PICKED_YET;
            *game = i;
            players[indexFirstPlayer].game = i;
            players[indexSecondPlayer].game = i;
        }
        i = i + 1;
    }
    pthread_mutex_unlock(&lock);
}

/**
 * pokusi se k hraci najit volheho hrace, pokud najde tak vyvori novou hru
 *
 * @param id hrace
 * @return true pokud se podarilo vytvorit hru, jinak false
 **/
bool attemptGameStart(int id) {
    //printf("Pokus o zanuti hry\n");
    //strtok(message, ":");
    //strtok(NULL, ":");
    //int id = atoi(strtok(NULL, ":"));
    //printf("id: %d\n", id);
    int freePlayer = getFreePlayer(id);
    int navrat = false;
    if (freePlayer != FAILURE_VALUE) {
        addNewRunningGame(freePlayer, id, &navrat);
        navrat = true;
    }
    return navrat;
}

/**
 * ziska id opponenta hrace co je v hre
 *
 * @param game index hry ve ktere hledame opponenta
 * @param id hrace ke kteremu hledame oppoenta
 * @return id opponenta
 **/
int getIdOfOpponent(int game, int id) {
    pthread_mutex_lock(&lock);
    if (runningGames[game].indexOfPlayer2 == id) {
        int navrat = runningGames[game].indexOfPlayer1;
        pthread_mutex_unlock(&lock);
        return navrat;
    } else {
        int navrat = runningGames[game].indexOfPlayer2;
        pthread_mutex_unlock(&lock);
        return navrat;
    }
}

/**
 * ziska socket hrace podle jeho id
 *
 * @param id hrace jehoz socket hledame
 * @return socket hrace
 **/
int getSocketOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].clientSocket;
    pthread_mutex_unlock(&lock);
    return navrat;
}

/**
 * nastavi tah hraci
 *
 * @param indexOfPlayer index hrace
 * @param turn tah co hrac zvolil
 **/
void setTurnOfPlayer(int indexOfPlayer, int turn) {
    pthread_mutex_lock(&lock);
    players[indexOfPlayer].turn = turn;
    pthread_mutex_unlock(&lock);
    /*if (runningGames[game].whoTurnedSooner == NOONE_PLAYED_YET) {
        runningGames[game].whoTurnedSooner = indexOfPlayer;
    }*/
}

/**
 * odnastavi tah hraci (nastavi zpatky na TURN_NOT_PICKED_YET)
 *
 * @param indexOfPlayer index hrace
 **/
void unSetTurnOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    players[indexOfPlayer].turn = TURN_NOT_PICKED_YET;
    pthread_mutex_unlock(&lock);
}

/**
 * funkce na zjisteni jestli oba hraci hrali, true pokud no, false pokud ne
 *
 * @param indexOfGame index hry, kde zjistujem jestli oba hraci hrali
 * @return true pokud oba hraci hrali, false pokud ne
 **/
bool bothPlayersHaveTurn(int indexOfGame) {
    pthread_mutex_lock(&lock);
    if (indexOfGame == FAILURE_VALUE) {
        pthread_mutex_unlock(&lock);
        return false;
    }
    int turnOfFirstPlayer = players[runningGames[indexOfGame].indexOfPlayer1].turn;
    int turnOfSecondPlayer = players[runningGames[indexOfGame].indexOfPlayer2].turn;
    pthread_mutex_unlock(&lock);
    return turnOfFirstPlayer != TURN_NOT_PICKED_YET && turnOfSecondPlayer != TURN_NOT_PICKED_YET;
}

/**
 * ziska indexy obou hracu v hre a da je do int ukazetelu predanych v parametrech
 *
 * @param indexOfGame index hry kde zjsitujem hrace
 * @param firstPlayer ukazatel do ktereho je ulozen index prevniho hrace
 * @param secondPlayer ukazatel do ktereho je ulozen index druheho hrace
 **/
void getIndexOfPlayers(int indexOfGame, int *firstPlayer, int *secondPlayer) {
    pthread_mutex_lock(&lock);
    *firstPlayer = runningGames[indexOfGame].indexOfPlayer1;
    *secondPlayer = runningGames[indexOfGame].indexOfPlayer2;
    pthread_mutex_unlock(&lock);
}

int getTurnOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    int navrat = players[indexOfPlayer].turn;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getGameOfPlayer(int indexOfPlayer) {
    pthread_mutex_lock(&lock);
    int navrat = players[indexOfPlayer].game;
    pthread_mutex_unlock(&lock);
    return navrat;
}

bool isFirstPlayer(int id, int game) {
    pthread_mutex_lock(&lock);
    bool navrat = runningGames[game].indexOfPlayer1 == id;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int freePlayer(int id) {
    pthread_mutex_lock(&lock);
    if (players[id].state == WAITING_VALUE) {
        pthread_mutex_unlock(&lock);
        return FAILURE_VALUE;
    }
    players[id].state = WAITING_VALUE;
    players[id].turn = TURN_NOT_PICKED_YET;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int unsetGame(int game) {
    pthread_mutex_lock(&lock);
    if (runningGames[game].indexOfPlayer1 == FREE_POSITION && runningGames[game].indexOfPlayer2 == FREE_POSITION) {
        pthread_mutex_unlock(&lock);
        return FAILURE_VALUE;
    }
    runningGames[game].indexOfPlayer1 = FREE_POSITION;
    runningGames[game].indexOfPlayer2 = FREE_POSITION;
    runningGames[game].firstPlayerScore = 0;
    runningGames[game].secondPlayerScore = 0;
    runningGames[game].numberOfPlayedRounds = 0;
    runningGames[game].numberOfStalemates = 0;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int getStalemates(int game) {
    pthread_mutex_lock(&lock);
    int navrat = runningGames[game].numberOfStalemates;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int sendPingToAllPlayers() {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i = i + 1) {
        if (players[i].state != FREE_POSITION) {
            sendPing(players[i].clientSocket);
        }
    }
    return SUCCESS_VALUE;
}

int setPingOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = FAILURE_VALUE;
    if (players[id].state != FREE_POSITION) {
        players[id].numberOfNotAnwseredPings = 0;
        navrat = SUCCESS_VALUE;
    }
    pthread_mutex_unlock(&lock);
}

bool checkName(char *name) {
    pthread_mutex_lock(&lock);
    int i = 0;
    bool navrat = false;
    while (i < MAX_NUMBER_OF_PLAYERS && !navrat) {
        if (players[i].state != FREE_POSITION && strcmp(players[i].name, name) == STRINGS_ARE_SAME) {
            navrat = true;
        } else {
            i = i + 1;
        }
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getNumberOfPings(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].numberOfPings;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getNumberOfPongs(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].numberOfPongs;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int setNumberOfPings(int id, int numberOfPings) {
    pthread_mutex_lock(&lock);
    players[id].numberOfPings = numberOfPings;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int setNumberOfPongs(int id, int numberOfPongs) {
    pthread_mutex_lock(&lock);
    players[id].numberOfPongs = numberOfPongs;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int disconnectPlayer(int id) {
    //printf("Odpojuji hrace: %s\n", players[id].name);
    pthread_mutex_lock(&lock);
    players[id].state = FREE_POSITION;
    players[id].index = FREE_POSITION;
    players[id].clientSocket = FREE_POSITION;
    players[id].turn = TURN_NOT_PICKED_YET;
    players[id].game = FREE_POSITION;
    players[id].connectionGood = NEUTRAL_VALUE;
    players[id].numberOfPings = 0;
    players[id].numberOfPongs = 0;

    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int setTimeSinceLastPing(int id, long time) {
    pthread_mutex_lock(&lock);
    players[id].timeSinceLastPing = time;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int removeGame(int id) {
    pthread_mutex_lock(&lock);
    runningGames[id].indexOfPlayer1 = FREE_POSITION;
    runningGames[id].indexOfPlayer2 = FREE_POSITION;
    runningGames[id].numberOfPlayedRounds = 0;
    runningGames[id].firstPlayerScore = 0;
    runningGames[id].secondPlayerScore = 0;
    runningGames[id].numberOfPlayedRounds = 0;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;

}

int infromOpponent(int id, int typeOfInfo) {
    int game = getGameOfPlayer(id);
    int opponentId = getIdOfOpponent(game, id);
    if (typeOfInfo == CEKAM_NA_SIGNAL) {
        char message[TYPE_ONE_MESSAGE_LENGTH] = "Mess:opponentConnectionProblems:\n";
        pthread_mutex_lock(&lock);
        players[opponentId].connectionGood = CEKAM_NA_SIGNAL;
        pthread_mutex_unlock(&lock);
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_ONE_MESSAGE_LENGTH);
    } else if (typeOfInfo == SUCCESS_VALUE) {
        char message[TYPE_TWO_MESSAGE_LENGTH] = "Mess:opponentConnectionGood:\n";
        pthread_mutex_lock(&lock);
        players[opponentId].connectionGood = SUCCESS_VALUE;
        pthread_mutex_unlock(&lock);
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_TWO_MESSAGE_LENGTH);
    } else if (typeOfInfo == FAILURE_VALUE) {
        char message[TYPE_TWO_MESSAGE_LENGTH] = "Mess:opponentConnectionFall:\n";
        pthread_mutex_lock(&lock);
        players[opponentId].state = WAITING_VALUE; //dani hrace jehoz oponent se odpojil zpatky do fronty
        players[opponentId].connectionGood = SUCCESS_VALUE;
        //players[opponentId].connectionGood = SUCCESS_VALUE;
        pthread_mutex_unlock(&lock);
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_TWO_MESSAGE_LENGTH);
    }
}

int setGameHalted(int game, bool halt) {
    pthread_mutex_lock(&lock);
    runningGames[game].gameHalted = halt;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

bool isGameHalted(int game) {
    pthread_mutex_lock(&lock);
    bool navrat = runningGames[game].gameHalted;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int infromOpponentGame(int id, int game, int typeOfInfo) {
    int opponentId = getIdOfOpponent(game, id);
    if (typeOfInfo == CEKAM_NA_SIGNAL) {
        char message[TYPE_ONE_MESSAGE_LENGTH] = "Mess:opponentConnectionProblems:\n";
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_ONE_MESSAGE_LENGTH);
    } else if (typeOfInfo == SUCCESS_VALUE) {
        char message[TYPE_TWO_MESSAGE_LENGTH] = "Mess:opponentConnectionGood:\n";
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_TWO_MESSAGE_LENGTH);
    } else if (typeOfInfo == FAILURE_VALUE) {
        char message[TYPE_TWO_MESSAGE_LENGTH] = "Mess:opponentConnectionFall:\n";
        pthread_mutex_lock(&lock);
        players[opponentId].state = WAITING_VALUE;
        pthread_mutex_unlock(&lock);
        sendMessage(getSocketOfPlayer(opponentId), message, TYPE_TWO_MESSAGE_LENGTH);
    }
}

int isInGame(int id) {
    pthread_mutex_lock(&lock);
    int navrat = FAILURE_VALUE;
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS / 2; i = i + 1) {
        if (runningGames[i].indexOfPlayer1 == id || runningGames[i].indexOfPlayer2 == id) {
            navrat = i;
        }
    }
    pthread_mutex_unlock(&lock);
    return navrat;
}

int getConnectionGoodOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].connectionGood;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int setConnectionGoodOfPlayer(int id, int connectionGood) {
    pthread_mutex_lock(&lock);
    players[id].connectionGood = connectionGood;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int checkPlayer(int id) {
    int state = players[id].state;
    int navrat = NEUTRAL_VALUE;
    if (FREE_POSITION != state) {
        //slabsi povahy, zakyrte si oci to co se tady prave bude dit je opradu desivy
        /*if (players[id].connectionGood == FAILURE_VALUE && ((players[id].numberOfPings == players[id].numberOfPongs) && ((getTimeInMili() - players[id].timeSinceLastPing) < TIME_FOR_ONE_PING))) {
            //printf("znovu pripojuji hrace: %s", players[id].name);
            pthread_mutex_lock(&lock);
            players[id].connectionGood = SUCCESS_VALUE;
            pthread_mutex_unlock(&lock);
            if (players[id].state == IN_GAME_VALUE) {
                infromOpponent(id, SUCCESS_VALUE);
            }
            navrat = SUCCESS_VALUE;
        }*/
        if (getConnectionGoodOfPlayer(id) == RECONNECT_VALUE && (((players[id].numberOfPings - players[id].numberOfPongs) >= MORE_PINGS) || ((getTimeInMili() - players[id].timeSinceLastPing) >= TIME_FOR_MORE_PINGS))) {
            //printf("Hraci %s je hodne, jeho internet to jumpoval\n", players[id].name);
            //disconnectPlayer(id);
            //setConnection(id, false);
            bool runningGame = !(getScoreOfFirstPlayer(players[id].game) != ROUNDS && getScoreOfSecondPlayer(players[id].game) != ROUNDS);
            if ((players[id].state == IN_GAME_VALUE || players[id].state == IN_GAME_WAITING_VALUE) && runningGame) {
                infromOpponent(id, FAILURE_VALUE);
                removeGame(players[id].game);
            }
            pthread_mutex_lock(&lock);
            players[id].connectionGood = FAILURE_VALUE;
            pthread_mutex_unlock(&lock);
            disconnectPlayer(id);
            navrat = FAILURE_VALUE;
        }
        //if ((players[id].connectionGood == SUCCESS_VALUE && ((players[id].numberOfPings != players[id].numberOfPongs) || ((getTimeInMili() - players[id].timeSinceLastPing) >= 1010)))) {
        if ((getConnectionGoodOfPlayer(id) == SUCCESS_VALUE && (getTimeInMili() - players[id].timeSinceLastPing) >= TIME_FOR_ONE_PING)) {
            //printf("hrac %s ma problem s pripojenim\n", players[id].name, getTimeInMili(), players[id].timeSinceLastPing);
            setConnection(id, false);
            if (players[id].state == IN_GAME_VALUE || players[id].state == IN_GAME_WAITING_VALUE) {
                infromOpponent(id, CEKAM_NA_SIGNAL);
            }
            pthread_mutex_lock(&lock);
            players[id].connectionGood = RECONNECT_VALUE;
            pthread_mutex_unlock(&lock);
            navrat = CEKAM_NA_SIGNAL;
        }
    } else {
        //int game = isInGame(id);
        //if (game != FAILURE_VALUE && !isGameHalted(game)) {
            //infromOpponentGame(id, game, CEKAM_NA_SIGNAL);
            //setGameHalted(game, true);
        //}
    }
    return navrat;
}

int getStateOfPlayer(int id) {
    pthread_mutex_lock(&lock);
    int navrat = players[id].state;
    pthread_mutex_unlock(&lock);
    return navrat;
}

int setStateOfPlayer(int id, int state) {
    pthread_mutex_lock(&lock);
    players[id].state = state;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

int setSocket(int id, int socket) {
    pthread_mutex_lock(&lock);
    players[id].clientSocket = socket;
    pthread_mutex_unlock(&lock);
    return SUCCESS_VALUE;
}

//bool getWhoIsPlayer(int id) {
//    return players[id].makerOfGame;
//}
