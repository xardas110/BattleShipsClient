#include "Menu.h"

Menu::Menu(const std::string &name, int width, int height, std::shared_ptr<Game> g)
:Window(name, width, height, false), gameButtons(g)
{
}

int Menu::OnControlInit()
{
    if (auto game = gameButtons.lock())
    { 
        game->OnControlInit();
        return 1;
    }
    else
        return 0;
}

void Menu::OnControlPress(WPARAM wParam, LPARAM lParam)
{
    if (auto game = gameButtons.lock())
        game->OnControlPress(wParam, lParam);
}
