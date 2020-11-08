#include "Board.h"
#include <iostream>
#include "../Engine/Debug.h"
#include <immintrin.h>
#include <inttypes.h>
#include <ctime>
Board::Board()
	:worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)),
	boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f)),
	gridLengthX(6), gridLengthY(6),
	gridSizeX(1.f), gridSizeY(1.f),
	playerScaleX(0.8f), playerScaleY(0.8f)
{
	BB.SetExtents(glm::vec3(gridLengthX * gridSizeX, gridLengthY * gridSizeY, 0.f));
	const auto gridXHalf = (gridLengthX * gridSizeX) * 0.5f;
	const auto gridYHalf = (gridLengthY * gridSizeY) * 0.5f;
	boardSpace = glm::translate(boardSpace, glm::vec3(gridXHalf, gridYHalf, 0.f));
	inverseBoard = glm::inverse(boardSpace);
}

Board::Board(const int numCol, const int numRow)
	:gridLengthX(numCol),gridLengthY(numRow), gridSizeX(1.f), gridSizeY(1.f), worldSpace(glm::mat4(1.f)), inverseWorld(glm::mat4(1.f)), playerScaleX(0.8f), playerScaleY(0.8f), boardSpace(glm::mat4(1.f)), inverseBoard(glm::mat4(1.f))
{
	BB.SetExtents(glm::vec3(gridLengthX * gridSizeX, gridLengthY * gridSizeY, 0.f));
	const auto gridXHalf = (gridLengthX * gridSizeX) * 0.5f;
	const auto gridYHalf = (gridLengthY * gridSizeY) * 0.5f;

	boardSpace = glm::translate(boardSpace, glm::vec3(gridXHalf, gridYHalf, 0.f));
	inverseBoard = glm::inverse(boardSpace);
	grid = Mesh::CreateGrid(gridLengthX, gridLengthY, gridSizeX, gridSizeY);
}


bool Board::SetTile(glm::vec3 posWS)
{

	auto localSpace = inverseWorld * glm::vec4(posWS, 1.f);
	auto boardSpace = this->boardSpace * glm::vec4(localSpace);
	boardSpace.y = (gridLengthY * gridSizeY) - boardSpace.y;//Gotta invert y so I can use it inside an array, where top of the board has y as 0 and not gridylength-1		
	//std::cout << localSpace.x << " " << localSpace.y << " " << localSpace.z << std::endl;
	//std::cout << boardSpace.x << " " << boardSpace.y << " " << boardSpace.z << std::endl;

	const int floorX = static_cast<int>(floorf(boardSpace.x));//this is safe, cause my raycast has a intersection point that can't be the same or larger than the board bounds
	const int floorY = static_cast<int>(floorf(boardSpace.y));//another option would be to add 9 bounding boxes for each tile, but that sure wont be as optimized as this
	//std::cout << floorX << " " << floorY << std::endl;

	//tiles[floorY][floorX] = player;
	//playerDrawList[player].push_back(GetTileCentrePositionWS(floorX, floorY));

	return true;
}


Board::~Board()
{

}

const iVec2D Board::TransformWStoBS(const glm::vec3 ws)
{
	std::cout << ws.x << "  " << ws.y << std::endl;
	auto localSpace = inverseWorld * glm::vec4(ws, 1.f);
	auto boardSpace = this->boardSpace * glm::vec4(localSpace);
	boardSpace.y = (gridLengthY * gridSizeY) - boardSpace.y;//Gotta invert y so I can use it inside an array, where top of the board has y as 0 and not gridylength-1		
	//std::cout << localSpace.x << " " << localSpace.y << " " << localSpace.z << std::endl;
	//std::cout << boardSpace.x << " " << boardSpace.y << " " << boardSpace.z << std::endl;

	const int floorX = static_cast<int>(floorf(boardSpace.x));//this is safe, cause my raycast has a intersection point that can't be the same or larger than the board bounds
	const int floorY = static_cast<int>(floorf(boardSpace.y));//another option would be to add 9 bounding boxes for each tile, but that sure wont be as optimized as this
	//std::cout << floorX << " " << floorY << std::endl;
	return { floorX,  floorY };
}

const glm::vec3 Board::TransformBStoWS(const glm::vec2 bs)
{
	const auto boardSpace = glm::vec4(bs, 0.f, 1.f);
	const auto WS = this->worldSpace * this->boardSpace * boardSpace;
	return glm::vec3(WS);
}

void Board::SetTranslate(glm::vec3 translate)
{
	worldSpace = glm::translate(worldSpace, translate);
	inverseWorld = glm::inverse(worldSpace);
	BB.SetCenter(worldSpace[3]);
	//PrintVector(BB.Extents);
}

void Board::SetScale(glm::vec3 scale)
{
	worldSpace = glm::scale(worldSpace, scale);
	inverseWorld = glm::inverse(worldSpace);
	const auto totalScale = glm::vec3(worldSpace[0][0], worldSpace[1][1], worldSpace[2][2]);
	BB.SetExtents(totalScale * glm::vec3(gridLengthX, gridLengthY, 0.f)); // haven't added a better function inside my bounding class, will do this later if I get time
	playerScaleX *= totalScale.x;
	playerScaleY *= totalScale.y;
}

void Board::SetTile(iVec2D posLS, DrawInfo hitinfo)
{
	gameDrawPositions[hitinfo].push_back({posLS.x, posLS.y});
}

const Bounding::Box Board::GetBoundingBox() const
{
	return BB;
}

const glm::mat4& Board::GetWorldSpace()
{
	return worldSpace;
}

void Board::Draw(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shaderTex, glm::mat4 projView)
{
	glm::mat4 boardMVP = projView * GetWorldSpace();
	shader->BindVec3("ambient", glm::vec3(0.2, 0.2f, 0.2f));
	shader->BindVec3("diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
	shader->BindVec3("specular", glm::vec3(1.f, 1.f, 1.f));
	shader->BindFloat("shininess", 64.f);
	shader->BindMat4("worldspace", GetWorldSpace());
	shader->BindMat4("MVP", boardMVP);
	grid->Draw(0x0001);//draw lines
	
	shaderTex->Use();	
	shaderTex->BindVec3("ambient", { 0.1f, 0.1f, 0.1f });
	shaderTex->BindVec3("specular", { 1.f, 1.f, 1.f });
	shaderTex->BindFloat("shininess", 2048);
	auto newLoc = glm::translate(GetWorldSpace(), textTranslate);
	newLoc = glm::scale(newLoc, textScale);
	const auto bMVP = projView * newLoc;
	shaderTex->BindMat4("worldspace", newLoc);
	shaderTex->BindMat4("MVP", bMVP);
	boardTextTex->BindTexture();
	boardTextMesh->Draw();
	for (int j = 0; j < _countof(gameDrawPositions); j++)
	{	
		gameTextures[j]->BindTexture();
		shaderTex->BindVec3("ambient", {0.1f, 0.1f, 0.1f});
		shaderTex->BindVec3("specular", { 1.f, 1.f, 1.f });
		shaderTex->BindFloat("shininess", 2048);
		for (int i = 0; i < gameDrawPositions[j].size(); i++)
		{
			auto model = glm::mat4(1.f);
			model = glm::translate(model, GetTileCentrePositionWS(gameDrawPositions[j][i].x, gameDrawPositions[j][i].y));
			model = glm::scale(model, glm::vec3(playerScaleX, playerScaleY, 1.f));
			shaderTex->BindMat4("worldspace", model);
			shaderTex->BindMat4("MVP", projView * model);
			gameMesh[j]->Draw();
		}
	}
	
}

void Board::Init()
{
	grid = Mesh::CreateGrid(gridLengthX, gridLengthY, gridSizeX, gridSizeY);
}


glm::vec3 Board::GetTileCentrePositionWS(const int x, const int y)
{
	auto const xCentre = (float)x + gridSizeX * 0.5f;
	const auto yCentre = (float)y + gridSizeY * 0.5f;
	auto tileCentreWS = glm::vec4(xCentre, (((float)gridLengthY * gridSizeY)) - yCentre, 0.f, 1.f);
	tileCentreWS = inverseBoard * tileCentreWS;
	tileCentreWS = worldSpace * tileCentreWS;
	return tileCentreWS;
}
