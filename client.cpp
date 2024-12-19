#define _WIN32_WINNT 0x0600


#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
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


// steps:
/*
    initialize winsock

    create socket

    connect to the server
    
    send recv

    close the socket
*/

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


int main(){
    if (!Initialize()) {
        cout << "Winsock initialization failed!" << endl;
        return 1;
    }

    // cout<< "Client program started" << endl;

    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == INVALID_SOCKET){
        cout<<"Invalid Socket Created!"<<endl;
        return 1;
    }

    // which server(IP) at which port
    int port = 12345;
    string serveraddress = "127.0.0.1";

    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));
        

    if(connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR){
        cout<<"Connection Error from server!"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout<<"Successfully connected to server!"<<endl;

    // send 0and recv
    string message = "Hello World!";
    int bytesent;
    bytesent = send(s, message.c_str(), message.size(), 0);
    if(bytesent == SOCKET_ERROR){
        cout<<"Send Failed!"<<endl;
    }
    closesocket(s);

    WSACleanup();
    return 0;
}