#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <map>
#include <vector>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

std::map<SOCKET, std::string> clientNames;
std::vector<SOCKET> clients; 
std::mutex clientsMutex;

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

// Trimite mesajul tuturor 
void broadcastMessage(const std::string& message, SOCKET sender) {
    std::lock_guard<std::mutex> lock(clientsMutex);  

    for (SOCKET client : clients) {
        if (client != sender) {  
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

void clientHandler(SOCKET clientSocket) {
    char buffer[1024];
    int recvResult;

    
    recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvResult <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[recvResult] = '\0';
    std::string clientName = buffer;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clientNames[clientSocket] = clientName;
        clients.push_back(clientSocket);
    }

    std::cout << "Clientul " << clientName << " s-a conectat.\n";

    while ((recvResult = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[recvResult] = '\0';
        std::string message = clientName + ": " + buffer;

        std::cout << message << "\n";  

        broadcastMessage(message, clientSocket);
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
        clientNames.erase(clientSocket);
    }

    closesocket(clientSocket);
}

int main() {
    initializeWinsock();

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Eroare la crearea socket-ului!\n";
        cleanUpWinsock();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Eroare la bind!\n";
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Eroare la listen!\n";
        closesocket(serverSocket);
        cleanUpWinsock();
        return 1;
    }

    std::cout << "Serverul rulează pe portul 9000...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Eroare la accept!\n";
            continue;
        }

        std::thread(clientHandler, clientSocket).detach();  // Thread pentru fiecare client
    }

    closesocket(serverSocket);
    cleanUpWinsock();
    return 0;
}
