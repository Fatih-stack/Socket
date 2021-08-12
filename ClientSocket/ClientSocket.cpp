// ClientSocket.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Client.h"
using namespace aricanli::network;

int main()
{
	Client client;
	int ret = client.initSocket();
	if (ret) return 0;

	client.initEnv(80, "127.0.0.1");
	bool res = client.SetBlocking(false);
	if (!res) return EXIT_FAILURE;
	ret = client.connectServer();
	if (ret) return 0;
	while(1)
	{
		client.receiveMSG();
		client.sendMSG();
	}
	return 0;
}
