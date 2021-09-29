#pragma once
#include "QuadTree.h"
#include "../include/glm/glm.hpp"
#include "../Engine/Camera.h"
#include "../Engine/Game.h"
#include "../Engine/Shader.h"
#include "../Engine/Skybox.h"
#include "../Engine/Texture.h"

class QuadTreeOGL : public Game
{
public:
	QuadTreeOGL() = delete;
	QuadTreeOGL(std::string name, int width, int height, bool vSync);
	
	virtual void OnKeyPressed(KeyEvent& e) override;
	
	virtual void OnKeyReleased(KeyEvent& e) override;
	
	virtual void OnUpdate(UpdateEvent& e) override;
	
	virtual void OnMouseMove(MouseMoveEvent& e) override;
	
	virtual void OnMouseClick(MouseClickEvent& e) override;
	
	virtual int	 OnLoad() override;
	
	virtual void OnRender() override;

private:
	void DrawQuadTree(std::shared_ptr<Shader> primitiveMaterialShader, const glm::mat4& projectView);

	std::shared_ptr<Shader> primitiveMaterialShader;
	
	std::shared_ptr<Shader> primitiveTextureShader;
	
	std::shared_ptr<Shader> skyboxShader;
	
	std::shared_ptr<Texture>testTex;
	
	std::unique_ptr<Mesh> RectangleMesh;
	std::unique_ptr<Mesh> PointMesh;
	std::unique_ptr<Mesh> FilledQuad;
	std::unique_ptr<Mesh> CircleMesh;
	
	std::unique_ptr<QuadTree> quadTree{nullptr};

	Poly::Types currentSelected = Poly::Rectangle;
	
	Skybox skybox;
	Camera camera;
	float mX, mY;

};

