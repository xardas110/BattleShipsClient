#pragma once
#include "Window.h"
#include <memory>
#include <string>
#include "Events.h"
//I wont inherit window here cause I might want more than 1 window later
class Game : public std::enable_shared_from_this<Game>
{
private:
	std::string				name;
	int						width, height, vSync;
	
public:
	Game(const std::string& name, int width, int height);
	std::shared_ptr<Window> win;
	int						Init();	
	virtual int				OnControlInit();
	virtual int				OnLoad();
	virtual void			OnUpdate(UpdateEvent& e);
	virtual void			OnRender();
	virtual void			OnKeyPressed(KeyEvent &e);
	virtual void			OnKeyReleased(KeyEvent& e);
	virtual void			OnMouseMove(MouseMoveEvent& e);
	virtual void			OnMouseClick(MouseClickEvent& e);
	virtual void			OnControlPress(WPARAM wParam, LPARAM lParam);
	virtual void			HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	
protected:
	
};

