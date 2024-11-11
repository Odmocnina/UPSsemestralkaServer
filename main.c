//
// Created by Michael on 08.11.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include <stdbool.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handle_client(void* client_socket) {
    int sock = *(int*)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Přijímání zpráv od klienta
    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Zpráva od klienta: %s\n", buffer);

        // Odeslání odpovědi zpět klientovi
        char* response = "jasne vole\n";
        send(sock, response, strlen(response), 0);
        int flag = 1;
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
    }

    close(sock);
    printf("Klient se odpojil.\n");
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Vytvoření socketu
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Chyba při vytváření socketu");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind (přiřazení portu k socketu)
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Chyba při bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Naslouchání na portu
    if (listen(server_socket, 5) < 0) {
        perror("Chyba při listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server běží na portu %d.\n", PORT);

    bool isRunning = true;

    // Hlavní smyčka pro přijímání klientů
    while (isRunning) {
        // Přijetí nového klienta
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {
            perror("Chyba při přijímání klienta");
        } else {

            printf("Nový klient připojen.\n");

            // Každému klientovi vytvoříme nové vlákno
            pthread_t thread_id;
            int *client_sock = malloc(sizeof(int));
            *client_sock = client_socket;
            pthread_create(&thread_id, NULL, handle_client, client_sock);
            pthread_detach(thread_id);
        }
    }

    close(server_socket);
    return 0;
}

