//
// Created by Michael on 13.11.2024.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "constants.h"
#include "messageHandeler.h"
#include "gameObjects.h"
#include "game.h"
#include "postMan.h"

/**
 * funkce na obsaouzeni zpravy typu logout
 *
 * @param message zprava
 * @return id hrace
 **/
int handlelogout(char *message) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    removePlayerFromGamersArray(atoi(token));
    return atoi(token);
}

/**
 * funkce na vytovreni zpravy co bude poslana jako odpoved na zpravu co prisla od klient, zprava je vetsinou totozn, do
 * zpravy je pridano OK, pri loginu je poslano s id, ktere bylo hraci poslano id ktere mu bylo serverem prirazeno, pri
 * reconnectu je poslan update hry
 *
 *
 * @param buffer buffer pro zpravu pro klient
 * @param type typ zpravy
 * @param id, id hrace co zpravu poslal
 **/
void makeMessage(char *buffer, char *type, int id) {
    //char idInChar = id;  //muj genius je tak velky, ze ma vlastni gravitacni pole
    if (strcmp(type, "login") == STRINGS_ARE_SAME) {
        sprintf(buffer, "Mess:%s:%d:\n", type, id);
    } else if (strcmp(type, "reconnect") == STRINGS_ARE_SAME) { //pokud prisel reconnect message je nutno slozit
        if (isInGame(id) != FAILURE_VALUE) {                    //zpravu s updatem hry, aby se pripadne zmeny v hre
            int winner = getWinnerLastRound(getGameOfPlayer(id));//hraci ukazali, na konec reconnect zpravy se da char
            char update;                                         //ktery kilentovi rekna co se stalo kdyz neprijimal
            if (winner == STALEMATE_VALUE) {
                update = 's';
            } else if (winner == id) {
                update = 'w';
            } else if (winner == NOONE_WON_YET) {
                update = 'n';
            } else {
                update = 'l';
            }
            sprintf(buffer, "Mess:%s:OK:%c:%d\n", type, update,
                    getTurnOfPlayer(getIdOfOpponent(getGameOfPlayer(id), id)));
        } else {
            sprintf(buffer, "Mess:%s:OK:\n", type);
        }
    } else {
        sprintf(buffer, "Mess:%s:OK:\n", type);
    }
}

/**
 * funkce co ziska z stringu cislo v int
 *
 *
 * @param str retezec co prevadime na string
 * @param result buffer pro cislo
 * @return int hodnta vyjadrujici jestli se ziskani intu podarilo
 **/
int stringToInt(char *str, int *result) {
    char *endptr;
    errno = 0; // Vycistime errno pred volánim strtol

    long value = strtol(str, &endptr, 10);

    // Kontrola chyb
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return FAILURE_VALUE; // Preteceni nebo podteceni
    }

    if (*endptr != '\0') {
        return FAILURE_VALUE; // retězec obsahuje nepovolene znaky
    }

    *result = (int)value;
    return SUCCESS_VALUE; // Úspěšná konverze
}

int handleLogin(char *message, int clientSocket) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    if (strlen(token) > 9) {
        return FAILURE_VALUE;
    }
    int navrat;
    bool nameAlreadyUsed = checkName(token);
    if (nameAlreadyUsed) {
        printf("Jmeno jiz pouzito\n");
        navrat = NAME_ALREADY_USED;
    } else {
        navrat = addPlayerToGamersArray(token, clientSocket);
        if (navrat == FAILURE_VALUE) {
            printf("Moc hracu\n");
        } else {
            printf("Hrac pridan\n");
            printf("Prihlasen: %s\n", token);
        }
    }

    return navrat;
}

bool isIdOk(int number) {
    return number >= 0 && number < MAX_NUMBER_OF_PLAYERS && getConnectionGoodOfPlayer(number) == RECONNECT_VALUE;
}

int handleReconnect(char *message, int clientSocket) {
    strtok(message, ":");
    strtok(NULL, ":");
    char *token = strtok(NULL, ":");
    int idInMessage;
    int navrat = FAILURE_VALUE;
    int result = stringToInt(token, &idInMessage);
    if (result != FAILURE_VALUE && isIdOk(idInMessage)) {
        setSocket(idInMessage, clientSocket);
        setConnectionGoodOfPlayer(idInMessage, SUCCESS_VALUE);
        navrat = idInMessage;
    }

    return navrat;
}

bool isNumberOk(int number) {
    return number == ROCK_VALUE || number == SCISSORS_VALUE || number == PAPER_VALUE || number == LIZARD_VALUE || number == SPOCK_VALUE;
}

int handleTurn(char *message, int id) {
    strtok(message, ":");
    strtok(NULL, ":");
    int turn;
    int result = stringToInt(strtok(NULL, ":"), &turn);
    int navrat = FAILURE_VALUE;
    if (result == SUCCESS_VALUE && isNumberOk(turn)) {
        setTurnOfPlayer(id, turn);
        setStateOfPlayer(id, IN_GAME_WAITING_VALUE);
        navrat = SUCCESS_VALUE;
    }
    return navrat;
}

long getTimeInMili() {
    struct timeval tp;

    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

bool canClientSendMessage(int idOfPlayer, int messageType) {
    bool navrat = false;
    if (idOfPlayer != ID_NOT_GIVEN_YET && getStateOfPlayer(idOfPlayer) == WAITING_VALUE) {
        if (messageType == LOGOUT_VALUE) {
            navrat = true;
        }
    } else if (idOfPlayer != ID_NOT_GIVEN_YET && getStateOfPlayer(idOfPlayer) == IN_GAME_VALUE) {     //||
        if (messageType == TURN_VALUE) {
            navrat = true;
        }
    } else if (idOfPlayer != ID_NOT_GIVEN_YET && getStateOfPlayer(idOfPlayer) == IN_GAME_WAITING_VALUE) {
        if (messageType == READY_FOR_NEXT_ROUND_VALUE) {
            navrat = true;
        } else if (messageType == GAME_VALUE) {
            int scoreOfFirstPlayer = getScoreOfFirstPlayer(getGameOfPlayer(idOfPlayer));
            int scoreOfSecondPlayer = getScoreOfSecondPlayer(getGameOfPlayer(idOfPlayer));
            int indexFirstPlayer;
            int indexSecondPlayer;
            getIndexOfPlayers(getGameOfPlayer(idOfPlayer), &indexFirstPlayer, &indexSecondPlayer);
            if ((scoreOfFirstPlayer >= ROUNDS || scoreOfSecondPlayer >= ROUNDS) || (indexFirstPlayer == FREE_POSITION && indexSecondPlayer == FREE_POSITION)) {
                navrat = true;
            }
        }
    } else if (idOfPlayer == ID_NOT_GIVEN_YET) {
        if (messageType == LOGIN_VALUE) {
            navrat = true;
        }
    }
    if (messageType == PING_VALUE || messageType == RECONNECT_VALUE) {
        navrat = true;
    }
    return navrat;
}

//int getTurnFormMessage(char *message) {
//    return handleTurn(message);
//}

/**
 * funkce odstraneni neviditelnych znaku co jsou na levo zpravy
 *
 *
 * @param str string co chceme odriznout neviditelne znaky z leva
 * @return string timunty z leva
 **/
char *trimLeft(char *str) {
    // Kontrola prázdneho vstupu
    if (str == NULL) {
        return NULL;
    }

    // Najdi prvni znak, ktery neni bily
    while (isspace((unsigned char)*str)) {
        str = str + 1;
    }

    return str; // Vráti ukazatel na zacátek "orezaneho" retězce
}

/**
 * funkce co napocitak kolikrat je znak v retezci
 *
 *
 * @param str string kde pocitame char
 * @param character znak ktery v stringu pocitame
 * @return int kolikrat je char v stringu
 **/
int countChar(const char *str, char character) {
    int navrat = 0;
    while (*str != '\0') {  // Procházej retězec, dokud nedosáhneš konce
        if (*str == character) {
            navrat = navrat + 1;  // Zvyšime pocet, pokud je znak dvojtecka
        }
        str = str + 1;  // Posuň ukazatel na dalši znak
    }
    return navrat;
}

/**
 * funkce co zjsiti jestli je v zprave spravy pocet :, oddelovacich znaku v zprv
 *
 *
 * @param message zprv, kde kontrolujeme pocet :
 * @param number cislo kolik : ma v zprave mit
 * @return bool vyjadrujici jestli je v zprav sprvny pocet :
 **/
bool isThereCurrectNumber(char *message, int number) {
    return countChar(message, ':') == number;
}

//int handleReconnect(int id) {

//}

/**
 * funkce na jednoduche oblsouzeni zpravy, precte zpravu zpracu je ji a naplni buffer pro zpravu zptky, funkce take
 * kontroluje jestli zprv vyhovuje protokolu, jestli ne tak vrati FAILURE_VALUE, jinak vrati hodntu rikjici co typ zprvy
 * je
 *
 *
 * @param message zprv kterou handalujem
 * @param sendBackMessage buffer pro zpravu co bude posln klientovi jako odpoved I<3Pilsner
 * @param clientSocket socket pres ktery prisla zprv
 * @param id id hrace co zprvu poslal
 * @return typ zpravy vyjadren v int
 **/
int handleMessage(char *message, char *sendBackMessage, int clientSocket, int *id) {
    message = trimLeft(message);
    if (strncmp(message, "Mess:", LENGTH_OF_MESSAGE_SIGNATURE + 1) != 0) {
        printf("Zprava neni validni\n");
        return FAILURE_VALUE;
    }

    char fullMessageForInspection[strlen(message)];
    strncpy(fullMessageForInspection, message, strlen(message));

    //printf("Zprava je validni\n");
    int i = START_OF_MESSAGE;
    int j = 0;
    int navrat = FAILURE_VALUE;
    char typeOfMessage[MAXIMAL_LENGHT_OF_MESSAGE_TYPE];
    while (fullMessageForInspection[i] != '\n') {
        if (fullMessageForInspection[i] == ':') {  // Pokud narazime na strednik, ukoncime parsováni
            typeOfMessage[j] = '\0';  // Ukoncime slov
            if (strcmp(typeOfMessage, "login") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 3)
                                                                    && canClientSendMessage(*id, LOGIN_VALUE)) {
                usleep(500000);
                *id = handleLogin(fullMessageForInspection, clientSocket);
                if (*id == NAME_ALREADY_USED) {
                    makeMessage(sendBackMessage, "login", -1);
                    navrat = NAME_ALREADY_USED;
                } else if (*id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "login", *id);
                    navrat = LOGIN_VALUE;
                } else {
                    navrat = FAILURE_VALUE;
                }
            } else if (strcmp(typeOfMessage, "logout") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 3)
                                                                        && canClientSendMessage(*id, LOGOUT_VALUE)) {
                int id = handlelogout(fullMessageForInspection);
                if (id != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "logout", id);
                }
                navrat = LOGOUT_VALUE;
            } else if (strcmp(typeOfMessage, "turn") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 3)
                                                                        && canClientSendMessage(*id, TURN_VALUE)) {
                int turn = handleTurn(fullMessageForInspection, *id);
                if (turn != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "turn", turn);
                    navrat = TURN_VALUE;
                }
            } else if (strcmp(typeOfMessage, "readyForNextRound") == STRINGS_ARE_SAME
                        && isThereCurrectNumber(message, 3) && canClientSendMessage(*id, READY_FOR_NEXT_ROUND_VALUE)) {
                makeMessage(sendBackMessage, "readyForNextRound", -1);
                setStateOfPlayer(*id, IN_GAME_VALUE);
                navrat = READY_FOR_NEXT_ROUND_VALUE;
            } else if (strcmp(typeOfMessage, "game") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 3)
                                                                        && canClientSendMessage(*id, GAME_VALUE)) {
                makeMessage(sendBackMessage, "game", -1);
                navrat = GAME_VALUE;
            } else if (strcmp(typeOfMessage, "ping") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 3)
                                                                    && canClientSendMessage(*id, PING_VALUE)) {
                setNumberOfPings(*id, getNumberOfPings(*id) + 1);
                setTimeSinceLastPing(*id, getTimeInMili());
                makeMessage(sendBackMessage, "pong", *id);
                navrat = PING_VALUE;
            } else if (strcmp(typeOfMessage, "reconnect") == STRINGS_ARE_SAME && isThereCurrectNumber(message, 2 + 1)
                                                                            && canClientSendMessage(*id, PING_VALUE)) {
                int result = handleReconnect(fullMessageForInspection, clientSocket);
                if (result != FAILURE_VALUE) {
                    makeMessage(sendBackMessage, "reconnect", result);
                    *id = result;
                    if (getStateOfPlayer(*id) == IN_GAME_VALUE || getStateOfPlayer(*id) == IN_GAME_WAITING_VALUE) {
                        infromOpponent(*id, SUCCESS_VALUE);
                    }
                    navrat = RECONNECT_VALUE;
                }
            }
        }
        typeOfMessage[j] = fullMessageForInspection[i];
        i = i + 1;
        j = j + 1;
    }

    return navrat;
}

/**
 * funkce na komplikovane oblsouzeni zpravy, napriklad pokud jsou 2 hraci v queue ta,k, jsou dani do hrz, obou jim je
 * poslno, ze se jde gming
 *
 *
 *
 *
 * I<3Pilsner
 * @param clientSocket socket pres ktery prisla zprv
 * @param id id hrace co zprvu posll
 * @param messageType typ zpravy
 * @param returnValue asi vysledek sendu, moc nevim proc to tam je
 * @return int cislo jestli se cel funkce spachl
 **/
int handleMessageComplicated(int clientSocket, int id, int messageType, int *returnValue) {
    int game;
    int opponetId;
    if (messageType == LOGIN_VALUE) {
        bool isGame = attemptGameStart(id);
        game = getGameOfPlayer(id);
        if (isGame != false) {
            char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
            opponetId = getIdOfOpponent(game, id);
            *returnValue = sendMessage(clientSocket, gameBegin, LENGHT_OF_START_GAME_MESSAGE);
            sendMessage(getSocketOfPlayer(opponetId), gameBegin, LENGHT_OF_START_GAME_MESSAGE);
        }
    }
    if (messageType == TURN_VALUE) {
        game = getGameOfPlayer(id);
        opponetId = getIdOfOpponent(game, id);
        if (bothPlayersHaveTurn(game)) {        //jestli oba dva hraci hrali
            char messageForFirstPlayer[MAX_SIZE_OF_MESSAGE];
            char messageForSecondPlayer[MAX_SIZE_OF_MESSAGE];
            handleGame(game, messageForFirstPlayer, messageForSecondPlayer);
            if (isFirstPlayer(id, game)) {
                sendMessage(clientSocket, messageForFirstPlayer, strlen(messageForFirstPlayer));
                sendMessage(getSocketOfPlayer(opponetId), messageForSecondPlayer, strlen(messageForSecondPlayer));
            } else {
                sendMessage(getSocketOfPlayer(opponetId), messageForFirstPlayer,
                            strlen(messageForFirstPlayer));
                sendMessage(clientSocket, messageForSecondPlayer,
                            strlen(messageForSecondPlayer));
            }
            char bothPlayerTurn[LENGTH_OF_BOTHPLAYERS_TURED_MESSAGE] = "Mess:bothPlayerTurn:\n";
            sendMessage(clientSocket, bothPlayerTurn, strlen(bothPlayerTurn));
            sendMessage(getSocketOfPlayer(opponetId), bothPlayerTurn, strlen(bothPlayerTurn));
        }
    }
    if (messageType == GAME_VALUE) {
        game = getGameOfPlayer(id);
        unsetGame(game);
        freePlayer(id);
        game = FAILURE_VALUE;
        opponetId = FAILURE_VALUE;
        bool isGame = attemptGameStart(id);
        game = getGameOfPlayer(id);
        if (isGame != false) {
            char gameBegin[LENGHT_OF_START_GAME_MESSAGE] = "Mess:gameBegin:\n";
            opponetId = getIdOfOpponent(game, id);
            *returnValue = sendMessage(clientSocket, gameBegin, LENGHT_OF_START_GAME_MESSAGE);
            sendMessage(getSocketOfPlayer(opponetId), gameBegin, LENGHT_OF_START_GAME_MESSAGE);
        }
    }
    if (messageType == READY_FOR_NEXT_ROUND_VALUE) {
        game = getGameOfPlayer(id);
        opponetId = getIdOfOpponent(game, id);
        setPlayerReadyness(game);
        unSetTurnOfPlayer(id);
        unSetTurnOfPlayer(opponetId);
        if (getPlayerReadyness(game) == 2) {
            setWinnerLastRound(game, NOONE_WON_YET);
            resetPlayerReadyness(game);
            char gameBegin[24] = "Mess:bothPlayersReady:\n";
            usleep(50000);
            printf("soket prvniho: %d\n", clientSocket);
            printf("socket druhyho: %d\n", getSocketOfPlayer(opponetId));
            printf("id oppoennta: %d\n", opponetId);
            *returnValue = sendMessage(clientSocket, gameBegin, 24);
            sendMessage(getSocketOfPlayer(opponetId), gameBegin, 24);
        }
    }

    return SUCCESS_VALUE;
}
