#pragma once
#define WIN32_LEAN_AND_MEAN
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
	static Client* Get();
	int ShutDown();
	~Client();
};