#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
namespace aricanli {
	namespace network {
		class Client
		{
		private:
			SOCKADDR_IN srv;
			int nClientSocket;
			int nRet;
			char buffer[50];
		public:
			Client();
			Client(const Client&) = delete;
			Client& operator= (const Client&) = delete;
			~Client();
			int initSocket();
			void initEnv(int port, const char* ip);
			bool SetBlocking(bool isBlocking);
			int connectServer();
			void receiveMSG();
			void sendMSG();
		};
	}
}

