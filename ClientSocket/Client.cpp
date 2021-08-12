#include "Client.h"
using namespace aricanli::network;

//initialize
Client::Client()
{
    for (int i = 0; i < 50; i++)
        buffer[i] = 0;
    nRet = 0;
}

//close all sockets and delete pointer
Client::~Client()
{
    closesocket(nClientSocket);
    delete buffer;
}

//initialize WSA and open Socket if error occurs clean wsa and exit 
int Client::initSocket()
{
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        std::cout << "WSAStartup failed" << std::endl;
        WSACleanup();
        return (EXIT_FAILURE);
    }
    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nClientSocket < 0)
    {
        std::cout << "Socket call failed" << std::endl;
        WSACleanup();
        return (EXIT_FAILURE);
    }
    return 0;
}

//init environment for socket(ip address, port, family)
void Client::initEnv(int port, const char *ip)
{
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = inet_addr(ip);
    memset(&(srv.sin_zero), 0, 8);
}

//Sets socket blocking or nonblocking mode
bool Client::SetBlocking(bool isBlocking)
{
    u_long nonBlock = 0;
    u_long Block = 1;
    int res = ioctlsocket(nClientSocket, FIONBIO, isBlocking ? &Block : &nonBlock);
    if (res != 0) {
        std::cout << "ioctlsocket call failed" << std::endl;
        WSACleanup();
        return false;
    }
    std::cout << "ioctlsocket call passed" << std::endl;
    return true;
}

//connects to server according to given ip and port 
int Client::connectServer()
{
    nRet = connect(nClientSocket, (SOCKADDR*)&srv, sizeof(srv));
    if (nRet < 0) {
        std::cout << "Connect failed" << std::endl;
        WSACleanup();
        return (EXIT_FAILURE);
    }
    return 0;
}

//receives message from server and shows it on terminal
void Client::receiveMSG()
{
    std::cout << "Enter a key to read message : ";
    getchar();
    recv(nClientSocket, buffer, 50, 0);
    std::cout << "Message from server => " << buffer << std::endl;
}

//sends message to server
void Client::sendMSG()
{
    std::cout << "Now send your message to server : ";
    fgets(buffer, 50, stdin);
    send(nClientSocket, buffer, 50, 0);
}
