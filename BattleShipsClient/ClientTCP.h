#pragma once

#include <thread>
#include <string>
#include <WinSock2.h>
#include "Json.h"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class Client
{
private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	std::thread* listenTh = nullptr;
public:
	const SOCKET& GetSocket() const;
	int Init(const std::string IP);
	int Send(const char* data);
	int WaitEvent(int event, const size_t waitMS, Json& eventData);
	static Client* Get();
	int ShutDown();
	~Client();
};