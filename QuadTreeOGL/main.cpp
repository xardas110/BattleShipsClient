#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include "QuadTreeOGL.h"
#include "../Engine/Application.h"
#include "../Engine/CMD.h"

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
#define DEBUG   //Leaving this on for now
#ifdef DEBUG
    RedirectIOToConsole();
#endif // DEBUG

    {
        std::shared_ptr<QuadTreeOGL> bsClient = std::make_shared<QuadTreeOGL>("battleships", 1280, 720, false);
        Application::Create(hInstance);
        Application::Get().Run(bsClient);//run doesnt need a parameter, TODO: Add weakptr inside application class and remove parameter from Run()
    }
    Application::Destroy();
}
