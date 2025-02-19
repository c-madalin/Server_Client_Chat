#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

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

void clientHandler(SOCKET clientSocket, sockaddr_in clientAddr) {
    char buffer[1024];
    int recvResult;

    // Se primește mesajul de la client
    while ((recvResult = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recvResult] = '\0';

        // Folosim inet_ntop pentru a obține IP-ul clientului
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);

        std::cout << "Client (" << ipAddress << "): " << buffer << std::endl;
    }

    if (recvResult == SOCKET_ERROR) {
        std::cout << "Eroare la primirea datelor de la client!" << std::endl;
    }

    closesocket(clientSocket);
}

int main() {
    initializeWinsock();

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Crearea socket-ului a eșuat!" << std::endl;
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);  // Portul 9000
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Ascultă pe orice adresă

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding-ul a eșuat!" << std::endl;
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Eșec la ascultarea pe socket!" << std::endl;
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Serverul ascultă pe portul 9000..." << std::endl;

    while (true) {
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        // Acceptă o conexiune de la un client
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Conexiune client eșuată!" << std::endl;
            continue;
        }

        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "Client conectat: " << ipAddress << std::endl;

        // Creează un thread pentru a gestiona clientul
        std::thread(clientHandler, clientSocket, clientAddr).detach();  // Detach pentru a rula în fundal
    }

    closesocket(serverSocket);
    cleanUpWinsock();
    return 0;
}