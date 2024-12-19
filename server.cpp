#define _WIN32_WINNT 0x0600

#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int inet_pton(int af, const char *src, void *dst) {
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    strncpy(src_copy, src, INET6_ADDRSTRLEN);
    src_copy[INET6_ADDRSTRLEN] = '\0';

    if (WSAStringToAddressA((LPSTR)src_copy, af, NULL, (struct sockaddr *)&ss, &size) == 0) {
        switch (af) {
            case AF_INET:
                *(struct in_addr *)dst = ((struct sockaddr_in *)&ss)->sin_addr;
                return 1;
            case AF_INET6:
                *(struct in6_addr *)dst = ((struct sockaddr_in6 *)&ss)->sin6_addr;
                return 1;
        }
    }
    return 0;
}

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

int main() {
    if (!Initialize()) {
        cout << "Winsock initialization failed!" << endl;
        return 1;
    }

    cout << "Server Program" << endl;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed!" << endl;
        return 1;
    }

    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    if (inet_pton(AF_INET, "0.0.0.0", &serveraddr.sin_addr) != 1) {
        cout << "Failed to convert IP address!" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "Bind Failed!" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen Failed!" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on PORT: " << port << endl;

    SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Invalid Client Socket!" << endl;
    }

    char buffer[4096];
    int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

    string message(buffer, bytesrecvd);
    cout << "Message from client: " << message << endl;

    closesocket(clientSocket);
    closesocket(listenSocket);

    WSACleanup();
    return 0;
}
