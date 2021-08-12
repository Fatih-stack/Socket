#include "Server.h"
using namespace aricanli::network;

//initialize
Server::Server()
{
    srvServer = new SOCKADDR_IN;
}

//close all sockets and delete pointer
Server::~Server()
{
    closesocket(nSockServer);   //close server
    
    for (int index = 0; index < 5; index++) 
    {
        if (nArrClient[index] != 0) //if client is open 
        {
            closesocket(nArrClient[index]); //close client
            nArrClient[index] = 0;
        }
    }
    delete srvServer;
}

//receive new message and send message
void Server::ProcessNewMessage(int nClient, std::string msg, bool sendMsg)
{
    std::cout << "Processing new message from client : " << nClient << std::endl;
    char buff[257] = { 0 };
    int nRet = 0;
    if(!sendMsg)
        nRet = recv(nClient, buff, 256, 0);
    if (nRet < 0 && sendMsg)
    {
        std::cout << "Failed!!! Closing the connection for the client" << std::endl;
        closesocket(nClient);
        for (int index = 0; index < 5; index++)
        {
            if (nArrClient[index] == nClient)
            {
                nArrClient[index] = 0;
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
    else
    {
        if(!sendMsg)
        {
            std::cout << "The message come from client is : " << buff;
            std::cout << "******************************" << std::endl;
        }
        //send the response to client
        else 
        {
            send(nClient, msg.c_str(), 50, 0);
        }
    }
}

//check new client connection and add it to array
void Server::ProcessTheNewRequest(std::string buff)
{
    //New connection request
    if (FD_ISSET(nSockServer, &frServer))
    {
        int nLen = sizeof(SOCKADDR);
        int nClientSock = accept(nSockServer, NULL, &nLen);
        if (nClientSock > 0)
        {
            //Put it into the client fd_set
            int nInd;
            for (nInd = 0; nInd < 5; nInd++)
            {
                if (nArrClient[nInd] == 0)  //add new client
                {
                    nArrClient[nInd] = nClientSock;
                    send(nClientSock, buff.c_str(), 50, 0);     //send message
                    break;
                }
            }
            if (nInd == 5)
                std::cout << "No space for a new connection" << std::endl;
        }
    }
    else
    {
        //check clients
        for (int index = 0; index < 5; index++)
        {
            if (FD_ISSET(nArrClient[index], &frServer))
            {
                //Got the new message from client
                //recv new message
                //just queue that for new workers of your server to fullfill the request
                ProcessNewMessage(nArrClient[index], buff, true);
            }
        }
    }
}

//initialize WSA and open Socket if error occurs clean wsa and exit 
void Server::initializeSocket() 
{
    WSAData wSockData;
    WORD dllVersion = MAKEWORD(2, 2);
    long success = WSAStartup(dllVersion, &wSockData);	//Start Winsock DLL
    if (success < 0)
    {
        std::cout << "WSA is failed" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "WSA is initialized " << success << std::endl;
    nSockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSockServer < 0) {
        std::cout << "Socket not opened!!!" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "Socket is opened successfully " << nSockServer << std::endl;
}

//init environment for socket(ip address, port, family)
void Server::initEnv(int port, const char *ip)
{
    srvServer->sin_family = AF_INET;
    srvServer->sin_port = htons(port);
    srvServer->sin_addr.s_addr = inet_addr(ip);
    memset(&(srvServer->sin_zero), 0, 8);
}

//set socket option controls socket behavior
//provides to reuse address and port, prevent error "address is already in use" 
void Server::setSockOpt()
{
    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    nRetServer = setsockopt(nSockServer, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
    if (!nRetServer) std::cout << "The setsockopt call successful." << std::endl;
    else {
        std::cout << "The setsockopt call failed." << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

//Sets socket blocking or nonblocking mode
bool Server::SetBlocking(bool isBlocking)
{
    u_long nonBlock = 1;
    u_long Block = 0;
    nRetServer = ioctlsocket(nSockServer, FIONBIO, isBlocking ? &Block : &nonBlock);
    if (nRetServer != 0) {
        std::cout << "ioctlsocket call failed" << std::endl;
        WSACleanup();
        return false;
    }
    std::cout << "ioctlsocket call passed" << std::endl;
    return true;
}

//after socket is created bind socket to ip and port
void Server::bindSock()
{
    nRetServer = bind(nSockServer, (SOCKADDR*)srvServer, sizeof(SOCKADDR));
    if (nRetServer < 0) {
        std::cout << "Failed to bind to local port" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else std::cout << "Successfully bind to local port " << nRetServer << std::endl;
}

//wait for client to make connection with server
//MaxClient specifies that maximum client number
void Server::listenReq(int MaxClient)
{
    nRetServer = listen(nSockServer, MaxClient);
    if (nRetServer < 0) {
        std::cout << "Failed to listen to local port" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else std::cout << "Started to listen local port " << nRetServer << std::endl;
}

void Server::initVarsFD()
{
    nMaxFdServer = nSockServer + 1;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
}

//clear file descriptor variables and set read and exception variables for open sockets
void Server::arrangeFD()
{
    FD_ZERO(&frServer);
    FD_ZERO(&fwServer);
    FD_ZERO(&feServer);
    FD_SET(nSockServer, &frServer);
    FD_SET(nSockServer, &feServer);

    for (int index = 0; index < 5; index++)
    {
        if (nArrClient[index] != 0)
        {
            FD_SET(nArrClient[index], &frServer);
            FD_SET(nArrClient[index], &feServer);
        }
    }
}

//check new request on port if there set file descriptor and send response to client
void Server::contProcessNewRequest()
{
    nRetServer = select(nMaxFdServer, &frServer, &fwServer, &feServer, &tv);
    if (nRetServer > 0) {
        std::cout << "Data on port----Processing now-----" << std::endl;
        for (int index = 0; index < 5; index++)
        {
            if (FD_ISSET(nArrClient[index], &frServer))
            {
                ProcessNewMessage(nArrClient[index], "", false);
            }
        }
        std::cout << "Enter message to client : ";
        char str[50] = { 0 };
        fgets(str, 50, stdin);
        ProcessTheNewRequest(str);
        if (FD_ISSET(nSockServer, &feServer))
        {
            std::cout << "There is an exception" << std::endl;
        }
        if (FD_ISSET(nSockServer, &fwServer))
        {
            std::cout << "Ready to write something" << std::endl;
        }
        if (FD_ISSET(nSockServer, &frServer))
        {
            std::cout << "Ready to read something. Something new come to the port" << std::endl;
        }
    }
    else if (nRetServer == 0) {
        //    std::cout << "Nothing on the port : " << std::endl;
    }
    else {
        std::cout << "Failed..." << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}