#include "QuadtreeOGL.h"

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
	quadTree = std::unique_ptr<QuadTree>(new QuadTree({ { 0.f, 0.f, 0.f, }, { 4.f, 4.f, 0.f } }));
	quadTree->SubDivide(1);
	quadTree->PrintAllNodes();
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
	

	std::vector<Rect> drawContainer;
	quadTree->GetAllNodes(drawContainer);

	for (auto &rect : drawContainer)
	{
		model = rect.GetTranslation();
		primitiveMaterialShader->BindMat4("MVP", projectView * model);
		RectangleMesh->Draw(GL_LINE_LOOP);
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
}

void QuadTreeOGL::OnMouseMove(MouseMoveEvent& e)
{

}

void QuadTreeOGL::OnMouseClick(MouseClickEvent& e)
{
}
