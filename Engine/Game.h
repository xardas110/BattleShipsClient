#pragma once
#include "Window.h"
#include <memory>
#include <string>

#include "Camera.h"
#include "Events.h"
#include "Mesh.h"
#include "BoundingShapes.h"
#include "Shader.h"
#include "Skybox.h"
#include "Texture.h"


struct Skybox;

class Game : public std::enable_shared_from_this<Game>
{
private:
	std::string				name;
	int						width, height, vSync;
	
public:
	Game(const std::string& name, int width, int height);
	std::shared_ptr<Window>		win;
	int							Init();	
	virtual int					OnControlInit();
	virtual int					OnLoad();
	virtual void				OnUpdate(UpdateEvent& e);
	virtual void				OnRender();
	virtual void				OnKeyPressed(KeyEvent &e);
	virtual void				OnKeyReleased(KeyEvent& e);
	virtual void				OnMouseMove(MouseMoveEvent& e);
	virtual void				OnMouseClick(MouseClickEvent& e);
	virtual void				OnControlPress(WPARAM wParam, LPARAM lParam);
	virtual void				HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	
	static void					AddToDrawList(RCircle* rCircle);
	static void					RemoveFromDrawList(RCircle* rCircle);

	static void					AddToDrawList(RRect* rRect);
	static void					RemoveFromDrawList(RRect* rRect);

	static void					AddToDrawList(RPoint* rPoint);
	static void					RemoveFromDrawList(RPoint* rPoint);

	static void					AddToDrawList(RBRect* rBRect);
	static void					RemoveFromDrawList(RBRect* rBRect);

	static void					AddToDrawList(RORect* roRect);
	static void					RemoveFromDrawList(RORect* roRect);
	
	static std::array<std::vector<Poly*>, Poly::Types::Size>	DrawList;
		
protected:

	std::unique_ptr<Mesh>		mesh[Poly::Types::Size];
	
	std::shared_ptr<Shader>		primitiveMaterialShader;

	std::shared_ptr<Shader>		primitiveTextureShader;

	std::shared_ptr<Shader>		skyboxShader;

	std::unique_ptr<Skybox>		skybox;
	
	Camera						camera;
};

