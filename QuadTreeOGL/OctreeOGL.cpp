#include "QuadtreeOGL.h"
#include "../Engine/Raycast.h"


QuadTreeOGL::QuadTreeOGL(std::string name, int width, int height, bool vSync)
	:Game(name, width, height), mX{ 0 }, mY{ 0 }
{
}

int QuadTreeOGL::OnLoad()
{
	Game::OnLoad();
	
	quadTree = std::make_unique<QuadTree>(QuadTree(RBRect::Create({ 0.f, 0.f, 0.f, }, { 4.f, 4.f, 0.f } )));

	quadTree->PrintAllQuads();
	//quadTree->SubDivide(2);
	glPointSize(5.f);
	
	return true;
}

void QuadTreeOGL::OnRender()
{
	Game::OnRender();
}


glm::vec3 QuadTreeOGL::Get3DMouseDirection(const float mX, const float mY)
{
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
	return glm::normalize(radyDir3);
}

void QuadTreeOGL::OnUpdate(UpdateEvent& e)
{
	Game::OnUpdate(e);
	
}

void QuadTreeOGL::OnMouseMove(MouseMoveEvent& e)
{
	mX = (float)e.xPos; mY = (float)e.yPos;
}

void QuadTreeOGL::OnMouseClick(MouseClickEvent& e)
{
	if (e.button == MouseClickEvent::Button::Left && e.state == MouseClickEvent::ButtonState::Pressed)
	{
		const glm::vec3 radyDir3 = Get3DMouseDirection(this->mX, this->mY);

		RayCast ray(camera.GetPosition(), radyDir3);
		std::cout << "mouse pressed" << std::endl;
		std::cout << "centre :" << quadTree->GetBounds()->E.x << " " << quadTree->GetBounds()->E.y;
		std::cout << "Ray dir: " << radyDir3.x << " " << radyDir3.y << " " << radyDir3.z << std::endl;

		Box box(quadTree->GetBounds()->C, quadTree->GetBounds()->E);

		float tMin = -FLT_MAX;
		if (ray.Intersect(box, tMin))
		{		
			glm::vec3 intersectPoint = camera.GetPosition() + (radyDir3 * tMin);
			
			//Sometimes this z value ends up as a weird number, investigate.
			intersectPoint.z = 0.f;

			switch (currentSelected)
			{
				case Poly::Types::Rectangle:
					{
						const auto rect = RRect::Create(intersectPoint, { 0.2f, 0.2f, 0.f });
						rect->GetRenderSettings()->SetDiffuse({ 0.7f, 0.1f, 0.1f });
						quadTree->InsertIfSpace(rect);
					}
					break;
					case Poly::Types::Point:
					{
						const auto rect = RPoint::Create(intersectPoint);
						rect->GetRenderSettings()->SetDiffuse({ 1.0f, 0.0f, 0.0f });
						quadTree->InsertIfSpace(rect);
					}
					break;
					case Poly::Types::Circle:
					{
						const auto rect = RCircle::Create(intersectPoint, 0.2f);
						rect->GetRenderSettings()->SetDiffuse({ 0.1f, 0.2f, 0.7f });
						quadTree->InsertIfSpace(rect);
					}
					break;
					default:
					break;
			}

			std::cout << "intersecting" << std::endl;
			std::cout << intersectPoint.x << " " << intersectPoint.y << " " << intersectPoint.z << std::endl;
		}
	}
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
	case GLFW_KEY_KP_ADD:
		camera.AddVelocityZ(-1.f);
		break;
	case GLFW_KEY_KP_SUBTRACT:
		camera.AddVelocityZ(1.f);
		break;
	case GLFW_KEY_1:
		currentSelected = Poly::Types::Point;
		break;
	case GLFW_KEY_2:
		currentSelected = Poly::Types::Rectangle;
		break;
	case GLFW_KEY_3:
		currentSelected = Poly::Types::Circle;
		break;
	case GLFW_KEY_R:
		quadTree->AddRandomizedPoints(2000);
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
	case GLFW_KEY_KP_ADD:
		camera.AddVelocityZ(0.f);
		break;
	case GLFW_KEY_KP_SUBTRACT:
		camera.AddVelocityZ(0.f);
		break;
	default:
		break;
	}
}