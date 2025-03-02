#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

void initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Eroare WSAStartup!\n";
        exit(1);
    }
}

void cleanUpWinsock() {
    WSACleanup();
}

// Ascultă mesajele de la server
void receiveMessages(SOCKET clientSocket) {
    char buffer[1024];
    int recvResult;

    while ((recvResult = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recvResult] = '\0';
        std::cout << buffer << std::endl;
    }

    std::cout << "Conexiunea cu serverul s-a intrerupt.\n";
}

int main() {
    initializeWinsock();

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Eroare la crearea socket-ului!\n";
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    inet_pton(AF_INET, "192.168.0.107", &serverAddr.sin_addr);  // Adresa IP a serverului

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Eroare la conectare!\n";
        closesocket(clientSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Conectat la server!\n";

    // Trimitere nume
    std::string name;
    std::cout << "Introdu numele tau: ";
    std::getline(std::cin, name);
    send(clientSocket, name.c_str(), name.size(), 0);

    // Pornim un thread separat pentru a asculta mesajele primite
    std::thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    // Trimitere mesaje către server
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    closesocket(clientSocket);
    cleanUpWinsock();
    return 0;
}
