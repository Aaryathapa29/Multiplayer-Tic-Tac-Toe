// server.cpp - Part 1

#include <iostream>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

#define PORT 8080

char board[9] = {
    '1','2','3',
    '4','5','6',
    '7','8','9'
};

SOCKET players[2];

void sendBoard()
{
    send(players[0], board, 9, 0);
    send(players[1], board, 9, 0);
}

bool checkWin(char p)
{
    int w[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };

    for(int i=0;i<8;i++)
    {
        if(board[w[i][0]]==p &&
           board[w[i][1]]==p &&
           board[w[i][2]]==p)
            return true;
    }

    return false;
}

bool draw()
{
    for(int i=0;i<9;i++)
    {
        if(board[i]>='1' && board[i]<='9')
            return false;
    }
    return true;
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server,(sockaddr*)&addr,sizeof(addr));
    listen(server,2);

    cout<<"Waiting for Player 1...\n";
    players[0]=accept(server,NULL,NULL);

    cout<<"Player 1 Connected\n";

    cout<<"Waiting for Player 2...\n";
    players[1]=accept(server,NULL,NULL);

    cout<<"Player 2 Connected\n";

    sendBoard();

       int turn = 0;
    int move;
    char symbol[2] = {'X', 'O'};

    while (true)
    {
        sendBoard();

        recv(players[turn], (char*)&move, sizeof(move), 0);

        if (move >= 0 && move < 9 && board[move] != 'X' && board[move] != 'O')
        {
            board[move] = symbol[turn];
        }
        else
        {
            continue; // Invalid move
        }

        if (checkWin(symbol[turn]))
        {
            sendBoard();

            send(players[turn], "WIN", 3, 0);
            send(players[1 - turn], "LOSE", 4, 0);
            break;
        }

        if (draw())
        {
            sendBoard();

            send(players[0], "DRAW", 4, 0);
            send(players[1], "DRAW", 4, 0);
            break;
        }

        turn = 1 - turn;
    }

    closesocket(players[0]);
    closesocket(players[1]);
    closesocket(server);

    WSACleanup();

    return 0;
}