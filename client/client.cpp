// client.cpp (Part 1)

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

void displayBoard(char board[])
{
    cout << "\n";
    cout << " " << board[0] << " | " << board[1] << " | " << board[2] << "\n";
    cout << "---|---|---\n";
    cout << " " << board[3] << " | " << board[4] << " | " << board[5] << "\n";
    cout << "---|---|---\n";
    cout << " " << board[6] << " | " << board[7] << " | " << board[8] << "\n";
    cout << "\n";
}

int main()
{
    WSADATA wsa;
    SOCKET clientSocket;
    sockaddr_in server;

    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        cout << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Connect to server
    if(connect(clientSocket,
              (sockaddr*)&server,
              sizeof(server)) < 0)
    {
        cout << "Connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server.\n";

    while(true)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket,
                                 buffer,
                                 BUFFER_SIZE,
                                 0);

        if(bytesReceived <= 0)
        {
            cout << "Disconnected from server.\n";
            break;
        }

        // Exit message
        if(strcmp(buffer, "EXIT") == 0)
        {
            cout << "Game Over.\n";
            break;
        }

        // Win/Lose/Draw messages
        if(strcmp(buffer, "WIN") == 0)
        {
            cout << "You Win!\n";
            break;
        }

        if(strcmp(buffer, "LOSE") == 0)
        {
            cout << "You Lose!\n";
            break;
        }

        if(strcmp(buffer, "DRAW") == 0)
        {
            cout << "Game Draw!\n";
            break;
        }

        // Display game board
        if(strlen(buffer) == 9)
        {
            displayBoard(buffer);

            int move;
            cout << "Enter position (1-9): ";
            cin >> move;

            move--;

            send(clientSocket,
                 (char*)&move,
                 sizeof(move),
                 0);
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
