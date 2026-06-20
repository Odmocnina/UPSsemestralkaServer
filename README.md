# Rock-Paper-Scissors-Lizard-Spock Multiplayer Server

This project is a multithreaded TCP server written in C that hosts multiplayer matches of **Rock-Paper-Scissors-Lizard-Spock**. It features a custom text-based communication protocol, automatic matchmaking, and network resilience mechanisms like pinging and reconnection.

Done as a semester work for KIV/UPS (intorduction to computer networks) (2024/2025) at FAV/ZCU in Pilsen.

@Author: Michael Hladky

## 🌟 Main Features

* **Multithreaded Architecture**: Utilizes `pthreads` to handle multiple client connections concurrently without blocking the main server execution.
* **Automatic Matchmaking**: Players connecting to the server are automatically placed in a lobby and paired with the next available opponent.
* **RPSLS Game Logic**: Full implementation of the extended Rock-Paper-Scissors-Lizard-Spock ruleset.
* **Network Resilience (Ping/Pong & Reconnect)**: The server actively monitors client connections using a ping system. It handles sudden disconnects gracefully and allows players to reconnect and resume their ongoing matches.
* **Custom Protocol**: Communication is handled via a structured, text-based messaging protocol (e.g., `Mess:login:ID:\n`, `Mess:turn:...`).

---

## ⚙️ Requirements and Dependencies

The server is built for Linux/Unix environments using standard POSIX libraries for sockets and threading.

* Standard C compiler (`gcc`)
* POSIX Threads library (`pthread`)
* Linux/Unix OS (uses `<sys/socket.h>`, `<netinet/in.h>`, etc.)

## 🛠️ Compilation

To compile the server, you need to link the `pthread` library. It is recommended to compile all `.c` files into a single executable:

```bash
gcc -Wall -Wextra -pthread -o rpsls_server *.c
```

## 🚀 Usage

The server is executed from the command line and requires exactly two arguments: the **IP address** to bind to, and the **Port number** (0 - 65535).

```bash
./rpsls_server <IP_ADDRESS> <PORT>
```

**Example:**
```bash
./rpsls_server 127.0.0.1 10000
```

---

## 📁 Project Structure (Modules)

The codebase is modular, separating network management from game logic:

* **`main.c`**
  The entry point. Validates command-line arguments, initializes the server socket, binds to the specified IP/Port, and spawns the main listener loop and monitoring threads.
* **`postMan.c` & `postMan.h`**
  Handles the low-level TCP socket communication. Manages client threads (`clientHandler`), receives raw data, sends messages, and actively checks connection health (`checkPlayers`).
* **`messageHandeler.c` & `messageHandeler.h`**
  The protocol parser. Validates incoming text messages, extracts commands (login, turn, reconnect, ping, etc.), and routes them to the appropriate game state functions.
* **`gameObjects.c` & `gameObjects.h` & `player.h`**
  The state management core. Maintains arrays of connected `player` structures and active `lobby` (game) instances. It tracks scores, turns, readyness, and matchmaking pairs safely using mutex locks.
* **`game.c` & `game.h`**
  The core game engine. Evaluates the specific matchups (e.g., Lizard poisons Spock, Scissors cuts Paper) and determines the winner of each round.
* **`constants.h`**
  A centralized file containing all configuration macros, buffer sizes, protocol string lengths, and numeric state codes used across the server.

---

## 🔌 Protocol & Flow (Brief Overview)

1. **Connection**: Client connects and sends a `login` message.
2. **Matchmaking**: Server assigns an ID and pairs the player with another waiting client. Sends a `gameBegin` message to both.
3. **Gameplay**: Players send `turn` messages. Once both turns are received, the server evaluates the logic and sends back the `gameResult`.
4. **Next Round**: Players explicitly send a `readyForNextRound` message to proceed.
5. **Disconnection**: If a client fails to respond to pings, the server notifies the opponent and pauses the match until a `reconnect` is issued.