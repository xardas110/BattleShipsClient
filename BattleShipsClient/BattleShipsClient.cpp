#define WIN32_LEAN_AND_MEAN
#include "PlayerClient.h"


int main()
{
    PlayerClient pc;
    pc.Connect("127.0.0.1");
    pc.Run();
    system("pause");
}

