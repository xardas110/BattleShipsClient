#pragma once
#include <Windows.h>
#include <memory>
#include <string>
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include "../include/GLFW/glfw3.h"
#include "../include/GLFW/glfw3native.h"
#include <iostream>
#include <map>
#include "Events.h"
#include <unordered_map>
#include <CommCtrl.h>
class Game;
using WindowUMap = std::unordered_map<std::string, HWND>;
class Window : public std::enable_shared_from_this<Window>
{
	friend class Application;//Application have access to everything
	friend class Menu;
protected:
	HWND				hwnd; //unordered map of all HWND handles that belong to this window
	GLFWwindow*			renderWindow;//using a wrapper lib for ogl/windows connection for now because its cumbersome work to link them
	std::string			name;
	int					width, height;
	bool				vSync;
	std::unordered_map<std::string, HWND> controls;
public:
	Window() = delete;
	Window(const std::string& name, int width, int height, int vsync);
	//Window operator=(Window copy);
	Window& operator=(Window&& reference) = default;

	std::weak_ptr<Game> game;
	HWND				GetControl(const std::string &winName);
	
	virtual void		OnUpdate(UpdateEvent& e);
	virtual void		OnRender();
	virtual void		OnKeyPressed(KeyEvent& e);
	virtual void		OnKeyReleased(KeyEvent& e);
	virtual void		OnMouseMove(MouseMoveEvent& e);
	virtual void		OnMouseClick(MouseClickEvent& e);
	virtual void		HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void		OnControlPress(WPARAM wParam, LPARAM lParam);
	virtual int			OnControlInit();
	HWND				InitGLFW();
	GLFWwindow*			GetRenderWindow() const;
	int&				GetWidth();
	int&				GetHeight();
	//CreateWindowExA(NULL, "BUTTON", "Apply", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 200, 140, 50, 25, window->hwnd, (HMENU)200, Application::Get().hInstance, nullptr);
	int					CreateButton(const std::string& buttonname, int x, int y, int width, int height, int callbackval, DWORD style =	WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON);
	HWND				CreateComboBox(const std::string& name, int x, int y, int width, int height, int callbackval, DWORD style = CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE);
	HWND				CreateStaticText(const std::string& text, int x, int y, int width, int height, int callbackval, DWORD style = WS_CHILD | WS_VISIBLE | ES_LEFT);
	
};

