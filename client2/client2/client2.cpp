#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void initializeWinsock() {
    WSADATA wsaData;
    int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
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
        std::cout << "Crearea socket-ului a eșuat!" << std::endl;
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);  // Conectare la server pe portul 9000
    inet_pton(AF_INET, "192.168.0.100", &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Conectarea la server a eșuat!" << std::endl;
        closesocket(clientSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Conectat la server!" << std::endl;

    // Citim și trimitem numele către server
    std::string name;
    std::cout << "Introdu numele tau: ";
    std::getline(std::cin, name);
    send(clientSocket, name.c_str(), name.size(), 0);  // Trimitere nume la server

    // Citim și trimitem mesaje către server
    std::string message;
    std::cout << "Introdu un mesaj (sau 'exit' pentru a iesi): ";
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }

        send(clientSocket, message.c_str(), message.size(), 0);  // Trimitere mesaj la server
    }

    closesocket(clientSocket);
    cleanUpWinsock();
    return 0;
}
