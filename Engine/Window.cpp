#include "Window.h"
#include "Game.h"
#include "Application.h"

Window::Window(const std::string& name, int width, int height, int vsync)
:name(name), width(width), height(height), vSync(vsync)
{
}

HWND Window::GetControl(const std::string &winName)
{
    auto search = controls.find(winName);
    if (search != controls.end())
        return search->second;
    printf("Can't find window: %s", winName.c_str());
    return (HWND)NULL;
    
}

int Window::OnControlInit()
{
    return 0;
}

void Window::OnUpdate(UpdateEvent& e)
{
    if (auto Game = game.lock())
    {
        Game->OnUpdate(e);
    }
}

void Window::OnRender()
{
    if (auto Game = game.lock())
    {
        Game->OnRender();
    }
    else
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);	

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
}

void Window::OnKeyPressed(KeyEvent& e)
{
    if (auto Game = game.lock())
    {
        Game->OnKeyPressed(e);
    }
}

void Window::OnKeyReleased(KeyEvent& e)
{
    if (auto Game = game.lock())
    {
        Game->OnKeyReleased(e);
    }
}

void Window::OnMouseMove(MouseMoveEvent& e)
{
    if (auto Game = game.lock())
        Game->OnMouseMove(e);
}

void Window::OnMouseClick(MouseClickEvent& e)
{
    if (auto Game = game.lock())
        Game->OnMouseClick(e);
}

void Window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (auto Game = game.lock())
    {
        Game->HandleMessage(message, wParam, lParam);
    }
}

void Window::OnControlPress(WPARAM wParam, LPARAM lParam)
{
    std::cout << "wm command running from window" << std::endl;
    if (auto Game = game.lock())
    {
        Game->OnControlPress(wParam, lParam);
    }
}

HWND Window::InitGLFW()
{
    renderWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL); 
    if (!renderWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    hwnd = glfwGetWin32Window(renderWindow);
    return hwnd;
}


GLFWwindow* Window::GetRenderWindow() const
{
    return renderWindow;
}

int& Window::GetWidth()
{
    return width;
}

int& Window::GetHeight()
{
    return height;
}

int Window::CreateButton(const std::string& buttonname, int x, int y, int width, int height, int callbackval, DWORD style)
{
   HWND hwnd = CreateWindowExA(NULL, "BUTTON", buttonname.c_str(), style, x, y, width, height, this->hwnd, (HMENU)callbackval, Application::Get().hInstance, nullptr);
   if (!hwnd)
   { 
       std::cout << "Button creation failed: " << buttonname.c_str() << std::endl;
       return 0;
   }
   controls.insert(std::map<std::string, HWND>::value_type(buttonname, hwnd));
   return 1;
}

HWND Window::CreateComboBox(const std::string& name, int x, int y, int width, int height, int callbackval, DWORD style)
{
    HWND hwnd = CreateWindowExA(NULL, WC_COMBOBOXA, name.c_str(), style, x, y, width, height, this->hwnd, (HMENU)callbackval, Application::Get().hInstance, nullptr);

    if (!hwnd)
    {
        std::cout << "Failed to create combobox: " << GetLastError() << std::endl;
        return 0;
    }
    controls.insert(std::map<std::string, HWND>::value_type(name, hwnd));

    return hwnd;
}

HWND Window::CreateStaticText(const std::string& text, int x, int y, int width, int height, int callbackval, DWORD style)
{
    HWND hwnd = CreateWindowExA(NULL, "STATIC", text.c_str(), style, x, y, width, height, this->hwnd, (HMENU)callbackval, NULL, NULL);
    if (!hwnd)
    {
        std::cout << "Failed to create static text " << GetLastError() << std::endl;
        return 0;
    }
    controls.insert(WindowUMap::value_type(text, hwnd));
    return hwnd;
}
