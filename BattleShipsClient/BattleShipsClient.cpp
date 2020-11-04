#include <iostream>
#include "PlayerClient.h"

int main()
{
    PlayerClient pc;
    pc.Connect("192.168.2.248");
    pc.Run();
}

