#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void initializeWinsock() {
    WSADATA wsaData;
    int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsInit != 0) {
        std::cout << "eroare WSAStartup  " << wsInit << std::endl;
        exit(1);
    }
}

void cleanUpWinsock() {
    WSACleanup();
}

int main() {
    initializeWinsock();

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "nu merge socket-ul" << std::endl;
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);  // Portul 9000
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding-ul esuat!" << std::endl;
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "eroare la ascultarea pe socket!" << std::endl;
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Serverul asculta pe portul 9000..." << std::endl;

    std::vector<SOCKET> clientSockets;

    while (true) {
        SOCKET clientSocket;
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Conexiune client naspa, nu merge!" << std::endl;
            continue;
        }

        std::cout << "Client conectat!" << std::endl;
        clientSockets.push_back(clientSocket);

        char buffer[1024];
        int recvResult;

        while ((recvResult = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            buffer[recvResult] = '\0';
            std::cout << "Client: " << buffer << std::endl;

        }

        if (recvResult == SOCKET_ERROR) {
            std::cout << "Eroare la primirea datelor!" << std::endl;
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    cleanUpWinsock();
    return 0;
}
