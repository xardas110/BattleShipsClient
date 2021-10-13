#include "Game.h"
#include "Application.h"
#include "Mesh.h"
#include "Shader.h"
#include <array>

std::array<std::vector<Poly*>, Poly::Types::Size> Game::DrawList;


void Game::OnKeyPressed(KeyEvent &e)
{
	//printf("Key pressed from game");
}

void Game::OnKeyReleased(KeyEvent& e)
{
}

void Game::OnMouseMove(MouseMoveEvent& e)
{
	////printf("On Mouse move from game");
}

void Game::OnMouseClick(MouseClickEvent& e)
{
	//printf("Mouse clicked inside game");
}

void Game::OnControlPress(WPARAM wParam, LPARAM lParam)
{
}

void Game::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
}

void Game::AddToDrawList(RCircle* rCircle)
{
	DrawList[Poly::Types::Circle].push_back(rCircle);
}

void Game::RemoveFromDrawList(RCircle* rCircle)
{
	DrawList[Poly::Types::Circle]
	.erase(std::remove(DrawList[Poly::Types::Circle].begin(), DrawList[Poly::Types::Circle].end(), rCircle), DrawList[Poly::Types::Circle].end());
}

void Game::AddToDrawList(RRect* rRect)
{
	DrawList[Poly::Types::Rectangle].push_back(rRect);
}

void Game::RemoveFromDrawList(RRect* rRect)
{
	DrawList[Poly::Types::Rectangle]
	.erase(std::remove(DrawList[Poly::Types::Rectangle].begin(), DrawList[Poly::Types::Rectangle].end(), rRect), DrawList[Poly::Types::Rectangle].end());
}

void Game::AddToDrawList(RPoint* rPoint)
{
	DrawList[Poly::Types::Point].push_back(rPoint);
}

void Game::RemoveFromDrawList(RPoint* rPoint)
{
	DrawList[Poly::Types::Point]
	.erase(std::remove(DrawList[Poly::Types::Point].begin(), DrawList[Poly::Types::Point].end(), rPoint), DrawList[Poly::Types::Point].end());
}

void Game::AddToDrawList(RBRect* rBRect)
{
	DrawList[Poly::Types::RBRect].push_back(rBRect);
}

void Game::RemoveFromDrawList(RBRect* rBRect)
{
	DrawList[Poly::Types::RBRect]
		.erase(std::remove(DrawList[Poly::Types::RBRect].begin(), DrawList[Poly::Types::RBRect].end(), rBRect), DrawList[Poly::Types::RBRect].end());
}

void Game::AddToDrawList(RORect* roRect)
{
	DrawList[Poly::Types::ORectangle].push_back(roRect);
}

void Game::RemoveFromDrawList(RORect* roRect)
{
	DrawList[Poly::Types::ORectangle]
		.erase(std::remove(DrawList[Poly::Types::ORectangle].begin(), DrawList[Poly::Types::ORectangle].end(), roRect), DrawList[Poly::Types::ORectangle].end());
}

void Game::AddToDrawList(RSegment* rSegment)
{
	DrawList[Poly::Types::Segment].push_back(rSegment);
}

void Game::RemoveFromDrawList(RSegment* rSegment)
{
	DrawList[Poly::Types::Segment]
		.erase(std::remove(DrawList[Poly::Types::Segment].begin(), DrawList[Poly::Types::Segment].end(), rSegment), DrawList[Poly::Types::Segment].end());
}


int Game::Init()
{
	win = Application::Get().CreateRenderWindow(name, width, height, vSync);
	win->game = this->shared_from_this();
	return 1;
}

int Game::OnControlInit()
{
	return 0;
}

int Game::OnLoad()
{
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 10.0f);
	camera.SetLookAt(camPos, glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
	camera.SetProjection(45.0f, (float)win->GetWidth() / (float)win->GetHeight(), 0.1f, 100.0f);

	primitiveMaterialShader = std::shared_ptr<Shader>(new Shader("Shaders/PrimitivesMaterial.vs", "Shaders/PrimitivesMaterial.fs"));
	primitiveTextureShader = std::shared_ptr<Shader>(new Shader("Shaders/PrimitivesTextured.vs", "Shaders/PrimitivesTextured.fs"));
	skyboxShader = std::shared_ptr<Shader>(new Shader("Shaders/Skybox.vs", "Shaders/Skybox.fs"));

	std::vector<std::string> textures
	{
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_left.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_right.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_up.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_down.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_front.png",
		"../Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_back.png"
	};

	skybox = std::make_unique<Skybox>(Skybox());
	skybox->Init(textures);

	mesh[Poly::Types::Circle] = Mesh::CreateCircle(16, 1.f);
	mesh[Poly::Types::Rectangle] = Mesh::CreateQuad();
	mesh[Poly::Types::ORectangle] = Mesh::CreateQuad();
	mesh[Poly::Types::Point] = Mesh::CreatePoint();
	mesh[Poly::Types::RBRect] = Mesh::CreateQuadLines();
	mesh[Poly::Types::Capsule2D] = Mesh::Create2DCapsule();
	mesh[Poly::Types::Segment] = Mesh::CreateSegment();
	
	return 1;
}

void Game::OnUpdate(UpdateEvent &e)
{
	camera.UpdatePosition(e.deltaTime);
}

void Game::OnRender()
{
	Application::ClearColor(0.f, 0.f, 0.f);
	Application::GetFrambufferSize(win->GetRenderWindow(), &win->GetWidth(), &win->GetHeight());//this function should run on resize, but im too lazy to add everything
	Application::SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Application::SetDepthMask(FALSE);
	Application::SetColorMask(TRUE, TRUE, TRUE, TRUE);


	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 project = camera.GetProjectionMatrix();
	const glm::mat4 projectView = project * view;
	const glm::mat4 skyboxView = glm::mat3(view);//removing translation vector for skybox
	const glm::mat4 skyBoxVP = project * skyboxView;

	skyboxShader->Use();
	skyboxShader->BindMat4("MVP", skyBoxVP);
	skybox->Draw(skyboxShader);

	primitiveMaterialShader->Use();
	primitiveMaterialShader->BindVec3("camPos", camera.GetPosition());

	glm::mat4 model(1.f);

	for (size_t i = 0; i < DrawList.size(); i++)
	{
		for (Poly *poly : DrawList[i])
		{
 			model = poly->GetTranslation();
			const auto* renderSettings = poly->GetRenderSettings();
			primitiveMaterialShader->BindMat4("MVP", projectView * model);
			primitiveMaterialShader->BindVec3("ambient", renderSettings->ambient);
			primitiveMaterialShader->BindVec3("diffuse", renderSettings->diffuse);
			primitiveMaterialShader->BindVec3("specular", renderSettings->specular);
			primitiveMaterialShader->BindFloat("shininess", renderSettings->shininess);
			mesh[i]->Draw(renderSettings->renderMode);
		}
	}

	
	Application::SwapBuffer(win->GetRenderWindow());
}

Game::Game(const std::string& name, int width, int height)
	:win(new Window(name, width, height, false)), width(width), height(height), vSync(false)
{
	
}
