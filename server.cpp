#include "iostream"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <process.h>
#include <vector>

// Classes
#include "src/classes/Client.h"

unsigned __stdcall ClientSession(void *params){

    clientSettings * data = (clientSettings *)params;

    // Creates "Client" object
    int index = data->clientSockets.size() - 1; // Gets last socket connected from the list

    Client client(data->clientSockets[index]);

    // Pushes client to connected list
    data->clientsConnected->push_back(client);

    // Send list to client
    client.sendConnectedClients(*data->clientsConnected);

    // Receive data from client
    client.receiveData();

    return 1;
}

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
    SOCKET serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(serverSocket == INVALID_SOCKET){
        cout << "Error when creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    cout << "Socket is OK!" << endl;

    // Bind the socket
    sockaddr_in service;
    service.sin_family = AF_INET;

    int port = 5555;
    
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);

    service.sin_port = htons(port);

    if(bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR){
        cout << "Bind failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "Bind is OK!" << endl;

    // Listen on the socket
    if(listen(serverSocket, 1) == SOCKET_ERROR)
        cout << "Error listening on socket!" << WSAGetLastError() << endl;

    cout << "Listen in OK, waiting for connections..." << endl;

    // Accept connections
    SOCKET acceptSocket;
    clientSettings * clientHandle;
    vector<Client> clientsConnected;

    clientHandle = new clientSettings; // Allocate on the heap so threads can access it before destroying it
    
    clientHandle->clientsConnected = &clientsConnected;

    while(acceptSocket = accept(serverSocket, NULL, NULL)){

        if(acceptSocket == INVALID_SOCKET){
            cout << "Accept failed: " << WSAGetLastError() << endl;
            WSACleanup();
            return -1;
        }

        clientHandle->clientSockets.push_back(acceptSocket); // Pushes to vector of SOCKETs

        // Create a new thread for each accepted client
        unsigned threadID;

        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, &ClientSession, (void*)clientHandle, 0, &threadID);

        cout << "Accepted connection" << endl;
    }

    delete clientHandle;
    
    // Disconnect
    system("pause");
    WSACleanup();

    return 0;

}