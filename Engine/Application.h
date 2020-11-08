#pragma once
//#include "../include/GLFW/glfw3.h"
#include "Game.h"
#include <vector>
#include <string>
#include <future>
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include "../include/GLFW/glfw3.h"
#include "../include/GLFW/glfw3native.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Windows.h>
#include "Menu.h"

#endif
//Head of the program, this has control of everything
class WindowWrapper;
class Application
{
private:
	Application(HINSTANCE hinst);
	void Init();
	
	int						SetGLFWCallback(std::shared_ptr<Window> w);
	static double			lastFrame;
	static double			totalTime;
	static uint64_t			frameNr;	
	std::future<void>		thread;
public:
	static					LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	//*Many of these functions are just wrapper functions that again uses a wrapper lib for window/ogl connection, the reason why I keep it like this is if I want to convert alot of this code w.o the use of wrapper libs, also only Application should have access to OGL*//
	static void				SwapBuffer(GLFWwindow* window);
	static void				SetViewport(int x, int y, int width, int height);
	static void				ClearColor(float r, float g, float b);
	static void				SetDepthMask(bool bval);
	static void				SetColorMask(bool r, bool g, bool b, bool a);
	static void				GetFrambufferSize(GLFWwindow* window, int* width, int* height);
	std::shared_ptr<Window> CreateRenderWindow(const std::string name, int width, int height, bool vsync);
	HINSTANCE				hInstance;
	static void				Create(HINSTANCE hinstace);
	/**For now this new window will just be used as menu, while CreateRenderWindow creates a window with OGL context**/
	static std::shared_ptr<Window>	CreateNewWindow(const std::string& winname, const int width = 400, const int height = 400);
	static int				CreateNewMenu(const std::string& winname,std::shared_ptr<Menu> &menu, std::shared_ptr<Game> game, const int width = 400, const int height = 400);
	static					Application& Get();
	int						Run(std::shared_ptr<Game> g);	
	static void				Destroy();
	static unsigned int		LoadCubeMapTextures(std::vector<std::string> textures);
	static WNDPROC			GetWNDPROC();
	std::shared_ptr<Window> callWindow;

};

