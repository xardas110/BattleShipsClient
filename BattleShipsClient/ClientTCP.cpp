#include "precompiled.h"
#include "ClientTCP.h"

Client* gs_app = nullptr;

const SOCKET& Client::GetSocket() const
{
    return ConnectSocket;
}

int Client::Init(const std::string IP)
{
    gs_app = this;
    WSADATA wsaData;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "Hey its me";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    // Resolve the server address and port
    iResult = getaddrinfo(IP.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }


    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return 1;
}

int Client::Send(const char* data)
{
    int iResult = send(ConnectSocket, data, strlen(data), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 0;
    }
    return 1;
}

int Client::WaitEvent(int event, const size_t waitMS, Json& eventData)
{
    const auto timeStart = std::chrono::system_clock::now();
    int iResult{ 0 };
    char data[DEFAULT_BUFLEN]{};
    do
    {
        iResult = recv(ConnectSocket, data, DEFAULT_BUFLEN,0);
        eventData = Json::Parse(data);
        if (eventData.GetType() != Json::Object)
            continue;
        if (!eventData.Contains("Status"))
            continue;
        for (const auto e : eventData["Status"])
        {
            if (e == event)
                return 1;
        }
        const auto timeEnd = std::chrono::system_clock::now();
        const auto timeElapsed = timeEnd - timeStart;
        if (timeElapsed > std::chrono::milliseconds(waitMS))
            return -1;
    } while (true);
    return 0;
}

Client* Client::Get()
{
    return gs_app;
}

int Client::ShutDown()
{
    // shutdown the connection since no more data will be sent
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }   
}

Client::~Client()
{
    closesocket(ConnectSocket);
    WSACleanup();
    delete listenTh;
}
