#include "QuadtreeOGL.h"
#include "../Engine/Raycast.h"

QuadTreeOGL::QuadTreeOGL(std::string name, int width, int height, bool vSync)
	:Game(name, width, height), mX{ 0 }, mY{ 0 }
{
}

int QuadTreeOGL::OnLoad()
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

	RectangleMesh = Mesh::CreateQuadLines();
	PointMesh = Mesh::CreatePoint();
	
	quadTree = std::make_unique<QuadTree>(QuadTree({ { 0.f, 0.f, 0.f, }, { 4.f, 4.f, 0.f } }));

	quadTree->Insert({{-3.99f, 3.99f, 0.f}});
	quadTree->PrintAllQuads();
	
	glPointSize(5.f);
	
	return true;
}

void QuadTreeOGL::OnRender()
{
	Application::ClearColor(0.f, 0.f, 0.f);
	Application::GetFrambufferSize(win->GetRenderWindow(), &win->GetWidth(), &win->GetHeight());//this function should run on resize, but im too lazy to add everything
	Application::SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Application::SetDepthMask(FALSE);
	Application::SetColorMask(TRUE, TRUE, TRUE, TRUE);

	
	glm::mat4 model(1.f);
	const glm::mat4 view = camera.GetViewMatrix();
	const glm::mat4 project = camera.GetProjectionMatrix();
	const glm::mat4 projectView = project * view;
	const glm::mat4 skyboxView = glm::mat3(view);//removing translation vector for skybox
	const glm::mat4 skyBoxVP = project * skyboxView;

	skyboxShader->Use();
	skyboxShader->BindMat4("MVP", skyBoxVP);
	skybox.Draw(skyboxShader);
	
	primitiveMaterialShader->Use();
	primitiveMaterialShader->BindVec3("camPos", camera.GetPosition());
	primitiveMaterialShader->BindVec3("ambient", glm::vec3(0.2, 0.2f, 0.2f));
	primitiveMaterialShader->BindVec3("diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	primitiveMaterialShader->BindVec3("specular", glm::vec3(1.f, 1.f, 1.f));
	primitiveMaterialShader->BindFloat("shininess", 64.f);

	primitiveMaterialShader->BindMat4("worldspace", model);
	primitiveMaterialShader->BindMat4("MVP", projectView * model);
	

	std::vector<Rect> drawContainerQuads;
	std::vector<Point> drawContainerPoints;
	
	quadTree->GetAllQuads(drawContainerQuads);
	quadTree->GetAllPoints(drawContainerPoints);
	
	for (auto &rect : drawContainerQuads)
	{
		model = rect.GetTranslation();
		primitiveMaterialShader->BindMat4("MVP", projectView * model);
		RectangleMesh->Draw(GL_LINE_LOOP);
	}

	for (auto& point : drawContainerPoints)
	{
		model = point.GetTranslation();
		std::cout << "point loacet :" <<  point.point.x << " " << point.point.y << std::endl;
		std::cout << "point size cointainer " <<drawContainerPoints.size() << std::endl;
		primitiveMaterialShader->BindMat4("MVP", projectView * model);
		primitiveMaterialShader->BindVec3("ambient", glm::vec3(1.0, 0.0f, 0.0f));
		primitiveMaterialShader->BindVec3("diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
		primitiveMaterialShader->BindVec3("specular", glm::vec3(1.f, 0.f, 0.f));
		PointMesh->Draw(GL_POINTS);
	}
	
	Application::SwapBuffer(win->GetRenderWindow());
}


void QuadTreeOGL::OnKeyPressed(KeyEvent& e)
{
	std::cout << "key pressed " << std::endl;
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

void QuadTreeOGL::OnKeyReleased(KeyEvent& e)
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

void QuadTreeOGL::OnUpdate(UpdateEvent& e)
{
	camera.UpdatePosition(e.deltaTime);
}

void QuadTreeOGL::OnMouseMove(MouseMoveEvent& e)
{
	mX = (float)e.xPos; mY = (float)e.yPos;
}

void QuadTreeOGL::OnMouseClick(MouseClickEvent& e)
{
	if (e.button == MouseClickEvent::Button::Left && e.state == MouseClickEvent::ButtonState::Pressed)
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

		std::cout << "mouse pressed" << std::endl;
		std::cout << "centre :" << quadTree->GetBounds().E.x << " " << quadTree->GetBounds().E.y;

		std::cout << "Ray dir: " << radyDir3.x << " " << radyDir3.y << " " << radyDir3.z << std::endl;

		Bounding::Box box(quadTree->GetBounds().C, quadTree->GetBounds().E);

		if (ray.Intersect(box, tMin))
		{
			std::cout << "intersecting" << std::endl;
			glm::vec3 intersectPoint = camera.GetPosition() + (radyDir3 * tMin);
			std::cout << intersectPoint.x << " " << intersectPoint.y << " " << intersectPoint.z << std::endl;
			//iVec2D pos2D = pClient.enemyBoard->TransformWStoBS(intersectPoint);
			//std::cout << "pos2d " << pos2D.x << " " << pos2D.y << std::endl;
			//std::cout << "pos 1d " << pClient.Convert2Dto1D(pos2D) << std::endl;
			//pClient.RequestShoot(pos2D);
			quadTree->Insert(intersectPoint);
		}
	}
}
