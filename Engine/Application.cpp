#include "Precompiled.h"
#include "Application.h"
#include "Window.h"
#include <map>
#include <iostream>

using WindowNameMap = std::map<std::string, std::shared_ptr<Window>>;//No need for hash table since there won't be many windows
using WindowCallback = std::map<HWND, std::shared_ptr<Window>>;
static Application* gs_S = nullptr;
double Application::lastFrame = glfwGetTime();
double Application::totalTime = 0.0;
uint64_t Application::frameNr = 0ULL;
WindowNameMap gs_WindowNameMap{};//<--TODO: CHANGE TO STD::PAIR
WindowCallback gs_WindowCallback{};//<----------------------
WNDPROC currWinProc;
HWND currHandle;

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//std::cout << hwnd << std::endl;
	std::shared_ptr<Window> window;
	{
		auto it = gs_WindowCallback.find(hwnd);
		if (it != gs_WindowCallback.end())
			window = it->second;
		else
			return CallWindowProc(currWinProc, hwnd, message, wParam, lParam);
		
	}
	switch (message)//I chose to use GLFW wrapper for all mouse and keycallbacks over the msg loop for now cuz it will save me lots of time. Every case that's not handled here will be handled by GLFW if it has the case built in
	{
		case WM_PAINT:
		{		
			const auto currentTime = glfwGetTime();
			const auto deltaTime = currentTime - Application::lastFrame;
			Application::totalTime += deltaTime;
			Application::lastFrame = currentTime;
			UpdateEvent e(deltaTime, Application::totalTime, Application::frameNr);
			window->OnUpdate(e);
			window->OnRender();
			Application::frameNr++;
		}
		break;	
		case WM_COMMAND:
		{
			window->OnControlPress(wParam, lParam);
		}
		break;
		case WM_SIZE:
		{
			std::cout << "resizing" << std::endl;
		}
		break;
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			DestroyWindow(hwnd);//Doing it simple for now, window should call OnClose() for proper solution later		
		}
		break;
		default:
		{
			window->HandleMessage(message, wParam, lParam); //Window can manually handle other cases
			return CallWindowProc(currWinProc, hwnd, message, wParam, lParam);
		}
	}	
}
	
static void error_callback(int error, const char* description)
{
	std::fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		KeyEvent eArgs(key, scancode, (KeyEvent::KeyState)GLFW_PRESS, mods);
		Application::Get().callWindow->OnKeyPressed(eArgs);
	}
	else if (action == GLFW_RELEASE)
	{
		KeyEvent eArgs(key, scancode, (KeyEvent::KeyState)GLFW_RELEASE, mods);
		gs_S->callWindow->OnKeyReleased(eArgs);
	}
}
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	MouseMoveEvent e(xpos, ypos);
	gs_S->callWindow->OnMouseMove(e);
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	MouseClickEvent e(button, action, mods);
	gs_S->callWindow->OnMouseClick(e);
}

void Application::Create(HINSTANCE hinstance)
{
	if (!gs_S)
	{
		gs_S = new Application(hinstance);
		gs_S->Init();
	}
}

std::shared_ptr<Window> Application::CreateNewWindow(const std::string& winname, const int width, const int height)
{
	auto window = std::make_shared<Window>(winname, width, height, false);
	window->renderWindow = glfwCreateWindow(width, height, winname.c_str(), nullptr, nullptr);
	window->hwnd = glfwGetWin32Window(window->renderWindow);	
	if (!window->hwnd)
	{ 
		std::cout << "Failed to create window: " << winname.c_str() << " Last error: " << GetLastError() << std::endl;
		return nullptr;
	}
	gs_WindowNameMap.insert(WindowNameMap::value_type(winname, window));
	gs_WindowCallback.insert(WindowCallback::value_type(window->hwnd, window));
	SetWindowLongPtr(window->hwnd, GWLP_WNDPROC, (DWORD64)WndProc);
	return window;
}

int Application::CreateNewMenu(const std::string& winname, std::shared_ptr<Menu> &menu, std::shared_ptr<Game> game, const int width, const int height)
{
	auto window = std::make_shared<Menu>(winname, width, height, game);
	window->renderWindow = glfwCreateWindow(width, height, winname.c_str(), nullptr, nullptr);
	window->hwnd = glfwGetWin32Window(window->renderWindow);
	if (!window->hwnd)
	{
		std::cout << "Failed to create menu: " << winname.c_str() << " Last error: " << GetLastError() << std::endl;
		return 0;
	}
	gs_WindowNameMap.insert(WindowNameMap::value_type(winname, window));
	gs_WindowCallback.insert(WindowCallback::value_type(window->hwnd, window));
	menu = window;
	SetWindowLongPtr(window->hwnd, GWLP_WNDPROC, (DWORD64)WndProc);
	window->OnControlInit();
	return 1;
}

Application& Application::Get()
{
	return *gs_S;
}

int Application::Run(std::shared_ptr<Game> g)
{
	if (g->Init() != 1) return -1; wpGame = g;
	
	if (SetGLFWCallback(g->win)!= 1) return -2;
	currHandle = g->win->hwnd;//has to be before Onload and after Init, TODO:find a better place for this
	currWinProc = (WNDPROC)GetWindowLongPtr(currHandle, GWLP_WNDPROC);
	if (g->OnLoad() != 1) return -3;
	SetWindowLongPtr(currHandle, GWLP_WNDPROC, (DWORD64)WndProc);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (GetMessageA(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
	return 1;
}

void Application::SwapBuffer(GLFWwindow* window)
{
	glfwSwapBuffers(window);
}

void Application::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void Application::ClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::SetDepthMask(bool bval)
{
	glDepthMask(bval);
}

void Application::SetColorMask(bool r, bool g, bool b, bool a)
{
	glColorMask(r, g, b, a);
}

void Application::GetFrambufferSize(GLFWwindow* window, int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
}

std::shared_ptr<Window> Application::CreateRenderWindow(const std::string name, int width, int height, bool vsync)
{
	WindowNameMap::iterator iter = gs_WindowNameMap.find(name);
	if (iter != gs_WindowNameMap.end())
		return iter->second;
	std::shared_ptr<Window> renderWindow = std::make_shared<Window>(name, width, height, false);

	renderWindow->renderWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (!renderWindow->renderWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwMakeContextCurrent(renderWindow->renderWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glfwSwapInterval(1);
	glfwSetKeyCallback(renderWindow->renderWindow, key_callback);
	glfwSetCursorPosCallback(renderWindow->renderWindow, mouse_callback);
	glfwSetMouseButtonCallback(renderWindow->renderWindow, mouse_button_callback);
	renderWindow->hwnd = glfwGetWin32Window(renderWindow->renderWindow);
	//std::cout << "hwnd from renderwindow " << renderWindow->hwnd << std::endl;
	gs_WindowNameMap.insert(WindowNameMap::value_type(name, renderWindow));
	gs_WindowCallback.insert(WindowCallback::value_type(renderWindow->hwnd, renderWindow));

	return renderWindow;
}

int Application::SetGLFWCallback(std::shared_ptr<Window> w)
{
	gs_S->callWindow = w;
	glfwSetKeyCallback(w->GetRenderWindow(), key_callback);
	glfwSetCursorPosCallback(w->GetRenderWindow(), mouse_callback);
	glfwSetMouseButtonCallback(w->GetRenderWindow(), mouse_button_callback);
	return 1;
}

void Application::Destroy()
{
	WindowNameMap::iterator iter = gs_WindowNameMap.begin();
	if (iter != gs_WindowNameMap.end())
	{
		glfwDestroyWindow(iter->second->GetRenderWindow());
	}
	
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

unsigned int Application::LoadCubeMapTextures(std::vector<std::string> textures)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		unsigned char* data = stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << textures[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

WNDPROC Application::GetWNDPROC()
{
	return currWinProc;
}

Application::Application(HINSTANCE hinst)
	:hInstance(hinst)
{
}

void Application::Init()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
}
