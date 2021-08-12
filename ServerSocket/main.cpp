// ServerSocket.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Server.h"

using namespace aricanli::network;

int main()
{
    Server server;
    server.initializeSocket();
    server.initEnv(80, "127.0.0.1");
    server.setSockOpt();
    bool res = server.SetBlocking(true);
    if (!res) return EXIT_FAILURE;
    server.bindSock();
    server.listenReq(5);
    while (1)
    {
        server.arrangeFD();
        server.contProcessNewRequest();
    }
    return 0;
}

