#include "../include/glm/gtc/matrix_transform.hpp"
#include "../Engine/Debug.h"
#include "../Engine/Application.h"
#include "BattleShipsOGLClient.h"
#include <iostream>


BattleShips::BattleShips(std::string name, int width, int height, bool vSync)
	:Game(name, width, height), mX{0}, mY{0}
{
}
void BattleShips::OnMouseClick(MouseClickEvent& e)
{
	if (e.button == MouseClickEvent::Button::Left && e.state == MouseClickEvent::ButtonState::Pressed && bPClient)
	{
		//Transfering x, y pixel cordinades to -1:1
		auto const xHN = ((mX * 2.f) / (float)(win->GetWidth())) - 1.f;
		auto const yHN = 1.f - ((mY * 2.f) / (float)win->GetHeight());
		auto inverseView = glm::inverse(camera.GetViewMatrix());
		auto inverseProj = glm::inverse(camera.GetProjectionMatrix());

		//std::cout << xHN << " " << yHN << std::endl;
		glm::vec4 hn(xHN, yHN, -1.f, 1.f); //z = -1.f because openGL uses a RH coordinate system
		glm::vec4 rayClip = inverseProj * hn;
		rayClip = glm::vec4(rayClip.x, rayClip.y, -1.f, 0.f);

		glm::vec4 rayDir4 = (inverseView * rayClip);
		glm::vec3 radyDir3(rayDir4.x, rayDir4.y, rayDir4.z);
		radyDir3 = glm::normalize(radyDir3);

		RayCast ray(camera.GetPosition(), radyDir3);
		float tMin = -FLT_MAX;
		
		if (ray.Intersect(pClient.enemyBoard->GetBoundingBox(), tMin))
		{
			glm::vec3 intersectPoint = camera.GetPosition() + (radyDir3 * tMin);
			iVec2D pos2D = pClient.enemyBoard->TransformWStoBS(intersectPoint);
			std::cout << "pos2d " << pos2D.x << " " << pos2D.y << std::endl;
			std::cout << "pos 1d " << pClient.Convert2Dto1D(pos2D) << std::endl;
			pClient.RequestShoot(pos2D);
		}	
	}
}

int BattleShips::OnLoad()
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
	skybox.Init(textures);

	pClient.PlayerClientListen();	
	WaitForSingleObject(pClient.hWaitBegin, INFINITE);
	pClient.enemyBoard = std::unique_ptr<Board>(new Board(pClient.numCol, pClient.numRow));
	pClient.myBoard = std::unique_ptr<Board>(new Board(pClient.numCol, pClient.numRow));
	const auto halfSizeX = pClient.numCol* 0.5f;
	const auto halfSizeY = pClient.numRow * 0.5f;
	pClient.enemyBoard->SetTranslate(glm::vec3(-(halfSizeX + .5f), 0.f, 0.f));
	pClient.myBoard->SetTranslate(glm::vec3(halfSizeX + .5f, 0.f, 0.f));
	pClient.RequestGameUpdate();
	WaitForSingleObject(pClient.hWaitUpdate, INFINITE);
	/*
	std::vector<glm::vec3> gameDrawPositions[Size];
	std::unique_ptr<Mesh> gameMesh[Size];
	std::shared_ptr<Texture> gameTextures[Size];
	*/
	pClient.enemyBoard->boardTextMesh = Mesh::CreateQuad();
	pClient.enemyBoard->boardTextTex = Texture::LoadTextureFromFile("../Textures/Enemy_Board_Text.png");
	pClient.enemyBoard->textTranslate = glm::vec3(0, halfSizeY+0.5, 0.f);
	pClient.enemyBoard->textScale = glm::vec3(4, 1, 1.f);
	pClient.enemyBoard->gameTextures[Board::Ship] = Texture::LoadTextureFromFile("../Textures/S.png");
	pClient.enemyBoard->gameTextures[Board::Hit] = Texture::LoadTextureFromFile("../Textures/O.png");
	pClient.enemyBoard->gameTextures[Board::Miss] = Texture::LoadTextureFromFile("../Textures/X.png");	
	pClient.enemyBoard->gameMesh[Board::Ship] = Mesh::CreateQuad();
	pClient.enemyBoard->gameMesh[Board::Hit] = Mesh::CreateQuad();
	pClient.enemyBoard->gameMesh[Board::Miss] = Mesh::CreateQuad();

	pClient.myBoard->boardTextMesh = Mesh::CreateQuad();
	pClient.myBoard->boardTextTex = Texture::LoadTextureFromFile("../Textures/Your_Board_Text.png");
	pClient.myBoard->textTranslate = glm::vec3(0, halfSizeY + 0.5, 0.f);
	pClient.myBoard->textScale = glm::vec3(4, 1, 1.f);
	pClient.myBoard->gameTextures[Board::Ship] = pClient.enemyBoard->gameTextures[Board::Ship];
	pClient.myBoard->gameTextures[Board::Hit] = pClient.enemyBoard->gameTextures[Board::Hit];
	pClient.myBoard->gameTextures[Board::Miss] = pClient.enemyBoard->gameTextures[Board::Miss];
	pClient.myBoard->gameMesh[Board::Ship] = Mesh::CreateQuad();
	pClient.myBoard->gameMesh[Board::Hit] = Mesh::CreateQuad();
	pClient.myBoard->gameMesh[Board::Miss] = Mesh::CreateQuad();

	pClient.statusTextMesh = Mesh::CreateQuad();
	pClient.statusTextTranslate = glm::vec3(0, -(halfSizeY + 0.5), 0.f);
	pClient.statusTextScale = glm::vec3(4, 1, 1.f);
	pClient.statusTextTex[(size_t)PlayerClient::GameStatus::Win] = Texture::LoadTextureFromFile("../Textures/You_Win.png");
	pClient.statusTextTex[(size_t)PlayerClient::GameStatus::Lose] = Texture::LoadTextureFromFile("../Textures/You_Lose.png");
	pClient.statusTextTex[(size_t)PlayerClient::GameStatus::Draw] = Texture::LoadTextureFromFile("../Textures/Draw.png");
	return 1;
}

void BattleShips::OnRender()
{
	Application::ClearColor(0.f, 0.f, 0.f);
	Application::GetFrambufferSize(win->GetRenderWindow(), &win->GetWidth(), &win->GetHeight());//this function should run on resize, but im too lazy to add everything
	Application::SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Application::SetDepthMask(FALSE);
	Application::SetColorMask(TRUE, TRUE, TRUE, TRUE);

	glm::mat4 model(1.f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 project = camera.GetProjectionMatrix();
	glm::mat4 projectView = project * view;
	glm::mat4 skyboxView = glm::mat3(view);//removing translation vector for skybox
	glm::mat4 skyBoxVP = project * skyboxView;

	skyboxShader->Use();
	skyboxShader->BindMat4("MVP", skyBoxVP);
	skybox.Draw(skyboxShader);
	primitiveTextureShader->Use();
	primitiveTextureShader->BindVec3("camPos", camera.GetPosition());
	primitiveMaterialShader->Use();
	primitiveMaterialShader->BindVec3("camPos", camera.GetPosition());
	pClient.myBoard->Draw(primitiveMaterialShader, primitiveTextureShader, projectView);
	primitiveTextureShader->Use();
	primitiveTextureShader->BindVec3("camPos", camera.GetPosition());
	primitiveMaterialShader->Use();
	primitiveMaterialShader->BindVec3("camPos", camera.GetPosition());
	pClient.enemyBoard->Draw(primitiveMaterialShader, primitiveTextureShader, projectView);
	
	for (auto& tex : pClient.statusDrawTex)
	{	
		primitiveTextureShader->Use();
		primitiveTextureShader->BindVec3("camPos", camera.GetPosition());
		glm::mat4 model(1.f);
		model = glm::translate(model, pClient.statusTextTranslate);
		model = glm::scale(model, pClient.statusTextScale);
		glm::mat4 boardMVP = projectView * model;
		primitiveTextureShader->BindVec3("ambient", glm::vec3(0.2, 0.2f, 0.2f));
		primitiveTextureShader->BindVec3("diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		primitiveTextureShader->BindVec3("specular", glm::vec3(1.f, 1.f, 1.f));
		primitiveTextureShader->BindFloat("shininess", 64.f);
		primitiveTextureShader->BindMat4("worldspace", model);
		primitiveTextureShader->BindMat4("MVP", boardMVP);
		tex->BindTexture();
		pClient.statusTextMesh->Draw();
		bPClient = false;
		pClient.Close();
	}
	Application::SwapBuffer(win->GetRenderWindow());
}

void BattleShips::JoinThreads()
{
	pClient.Close();
}

void BattleShips::OnUpdate(UpdateEvent& e)
{
	if (bPClient)
		pClient.RequestGamePing();
	camera.UpdatePosition(e.deltaTime);
}

void BattleShips::OnMouseMove(MouseMoveEvent& e)
{
	mX = (float)e.xPos; mY = (float)e.yPos;
}

void BattleShips::OnKeyPressed(KeyEvent& e)
{
	switch (e.key)
	{
	case GLFW_KEY_W:
		camera.AddVelocityY(1.f);
		break;
	case GLFW_KEY_S:
		camera.AddVelocityY(-1.f);
		break;
	case GLFW_KEY_D:
		camera.AddVelocityX(1.f);
		break;
	case GLFW_KEY_A:
		camera.AddVelocityX(-1.f);
		break;
	default:
		break;
	}
}

void BattleShips::OnKeyReleased(KeyEvent& e)
{
	switch (e.key)
	{
	case GLFW_KEY_W:
		camera.AddVelocityY(0.f);
		break;
	case GLFW_KEY_S:
		camera.AddVelocityY(0.f);
		break;
	case GLFW_KEY_D:
		camera.AddVelocityX(0.f);
		break;
	case GLFW_KEY_A:
		camera.AddVelocityX(0.f);
		break;
	default:
		break;
	}
}