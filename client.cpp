#include "iostream"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

using namespace std;

int main() {
    
    // Initialize WSA
    WSADATA wsaDATA;
    WORD version = MAKEWORD(2, 2);
    
    int wsaerr;

    wsaerr = WSAStartup(version, &wsaDATA);

    if(wsaerr != 0){
        cout << "Winsock dll not found!" << endl;
        return 0;
    }

    cout << "Winsock dll found!" << endl;
    cout << "Status: " << wsaDATA.szSystemStatus << endl;

    // Create a socket
    SOCKET clientSocket;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(clientSocket == INVALID_SOCKET){
        cout << "Error when creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    cout << "Socket is OK!" << endl;

    // Connect to the server
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;

    int port = 5555;

    InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);    

    clientService.sin_port = htons(port);

    if(connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR){
        cout << "Client connection failed!" << endl;
        WSACleanup();
        return -1;
    }

    cout << "Client connection is OK!" << endl;
    cout << "Client can start sending and receiving data..." << endl;

    // Receive content from the server
    char serverBuffer[200];

    int byteCount = recv(clientSocket, serverBuffer, 200, 0);

    // Check if the message arrived
    if(byteCount == 0)
        cout << "There was an error receiving the message!" << endl;

    cout << "Message received: " << serverBuffer << endl;

    // Chat to the server
    while(true){

        // Clears terminal
        // system("CLS");

        char buffer[200];

        cout << ":";

        cin.getline(buffer, 200);

        int byteCount = send(clientSocket, buffer, 200, 0);

        if(byteCount == 0){
            cout << "There was an error sending the message!" << endl;
            continue;
        }

        if(strcmp(buffer, "[quit]") == 0)
            break;

        cout << "Message sent successfully!" << endl;
    }

    // Disconnect


    system("pause");
    WSACleanup();

    return 0;

}