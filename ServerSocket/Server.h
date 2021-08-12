#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
namespace aricanli {
	namespace network {
		class Server
		{
		private:
			SOCKADDR_IN* srvServer;
			fd_set frServer, fwServer, feServer;
			TIMEVAL tv;
			int nRetServer;
			int nArrClient[5];

		public:
			int nMaxFdServer;
			int nSockServer;
			Server();
			Server(const Server&) = delete;
			Server& operator= (const Server&) = delete;
			~Server();
			void ProcessNewMessage(int nClient, std::string msg, bool sendMsg);
			void ProcessTheNewRequest(std::string buff);
			void initializeSocket();
			void initEnv(int port, const char* ip);
			void setSockOpt();
			bool SetBlocking(bool isBlocking);
			void bindSock();
			void listenReq(int MaxClient);
			void arrangeFD();
			void contProcessNewRequest();
		};
	}
}

