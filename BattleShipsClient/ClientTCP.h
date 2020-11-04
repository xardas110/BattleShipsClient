#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include "Json.h"
#include <mutex>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
class Client
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	std::mutex sendMTX;
	std::thread* listenTh = nullptr;
	void Listen();
	void Clean();
public:
	const SOCKET& GetSocket() const;
	int Init(const std::string IP);
	int Send(const char* data);
	int WaitEvent(int event, const size_t waitMS, Json& eventData);
	static Client* Get();
	static void OnListen();
	void Run();
	void IncomingData(char* data);
	int ShutDown();

	~Client();
};