#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void initializeWinsock() {
    WSADATA wsaData;
    int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData); // Inițializare Winsock
    if (wsInit != 0) {
        std::cout << "Eroare WSAStartup " << wsInit << std::endl;
        exit(1);
    }
}

void cleanUpWinsock() {
    WSACleanup();
}

int main() {
    initializeWinsock();

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Crearea socket-ului a esuat!" << std::endl;
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);  // Portul serverului
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // Adresa localhost

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Conectarea la server a esuat!" << std::endl;
        closesocket(clientSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Conectat la server!" << std::endl;

    // Citim și trimitem cuvinte către server
    std::string word;
    std::cout << "Introdu un cuvant (sau 'exit' pentru a iesi): ";
    while (true) {
        std::getline(std::cin, word);
        if (word == "exit") {
            break;
        }

        send(clientSocket, word.c_str(), word.size(), 0);  // Trimitere cuvânt la server
    }

    closesocket(clientSocket);
    cleanUpWinsock();
    return 0;
}
