#include "iostream"
#include <sstream>

using namespace std;

class Client {

    public:
    SOCKET clientSocket;
    string clientIP;
    int clientPort;

    Client(SOCKET socket){

        // Set client's accepted socket
        clientSocket = socket;

        // Send list of connected clients to client
        // sendConnectedClients();
        
        // Get client's ip and port
        getNetworkInfo();

        // Receive data from client
        // receiveData();
    }

    private:

        // Function to get client's ip and port
        void getNetworkInfo(){
            
            sockaddr address;
            int addressLength = sizeof(address);

            int peer = getpeername(clientSocket, &address, &addressLength);

            if(peer == SOCKET_ERROR){
                cout << "Failed to get client's IP address" << endl;
                return;
            }

            sockaddr_in * addressInternet = (sockaddr_in*)&address;

            // Gets client's ip
            string ip = inet_ntoa ( addressInternet->sin_addr );

            // Get client's port
            int port = ntohs ( addressInternet->sin_port );   

            cout << "Socket FD is " << socket << " " << ip << ":" << port << endl;

            clientIP = ip;
            clientPort = port;
        }

    public:

        // Function to receive messages from the client
        void receiveData(){
            
            while(true){

                // Create buffer
                char buffer[200];

                // Receive
                int byteCount = recv(clientSocket, buffer, 200, 0);

                // Tests
                if(byteCount == 0){
                    cout << "The client " << clientIP << ":" << clientPort << "has disconnected!" << endl;
                    return;
                }

                if(byteCount == SOCKET_ERROR){
                    cout << "An error has occured: " << WSAGetLastError() << endl;
                    cout << clientIP << ":" << clientPort << endl;
                    return;
                }

                cout << "Message received: " << buffer << endl;
            }
        }

        // Function to send list of connected clients to client
        void sendConnectedClients(vector<Client> list){
            
            // Index for iterating over the list of clients
            int clientIndex = 0;

            // Message
            stringstream messageStream;
            string message;

            for(Client client: list){

                // Formats the list string
                messageStream << clientIndex + 1 << " - " << client.clientIP << ":" << client.clientPort << "\n";

                clientIndex++;
            }

            message = messageStream.str();

            // Buffer
            char * buffer = new char [message.size() + 1]; 
            strcpy(buffer, message.c_str());

            // Send list
            int byteCount = send(clientSocket, buffer, sizeof(buffer), 0);

            if(byteCount == 0)
                cout << "There was an error sending the connected clients list!" << endl;

            return;
        }

};

// Set structure to handle client connection
typedef struct {

    // The socket connected
    vector<SOCKET> clientSockets;

    // Server parameter of connected clients
    vector<Client> * clientsConnected;
    
} clientSettings;