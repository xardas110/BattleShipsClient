#pragma once
#include "Window.h"
#include "Game.h"
#include <string>
class Menu : public Window
{
private:
public:
	std::weak_ptr<Game> gameButtons;
	Menu(const std::string &name, int width, int height, std::shared_ptr<Game> g);
	virtual int		OnControlInit();
	virtual void	OnControlPress(WPARAM wParam, LPARAM lParam) override;
};

