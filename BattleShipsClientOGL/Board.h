#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../Engine/Mesh.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Shader.h"
#include "../Engine/Material.h"
#include "../Engine/Texture.h"
#include "Vec.h"
#include "Json.h"
#include <memory>
#include <xmmintrin.h>
#include <vector>
#include <map>

class Board
{
	friend class Battleships;
	float playerScaleX, playerScaleY;
public:
	enum DrawInfo
	{
		Ship, Hit, Miss, Size //ship to be drawn first then miss or hit over the ship
	};
	
	Board();
	Board(const int x, const int y);
	~Board();
	//void SetTurn(glm::vec3 posWS);

	const iVec2D TransformWStoBS(const glm::vec3 ws);
	const glm::vec3 TransformBStoWS(const glm::vec2 bs);

	void SetTranslate(glm::vec3 translate);
	void SetScale(glm::vec3 scale);

	void SetTile(iVec2D posLS, DrawInfo hitinfo);

	const Bounding::Box GetBoundingBox() const;
	const glm::mat4& GetWorldSpace();

	void Draw(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shaderTex, glm::mat4 projview);
	void Init();
	const int gridLengthX; //------------------------------------------------------------------------------------------------------------------------------------------------------//
	const int gridLengthY;
	const float gridSizeX;
	const float gridSizeY; //------------------------------------------------------------------------------------------------------------------------------------------------------//
	std::vector<glm::vec2> gameDrawPositions[Size];//grid space positions
	std::unique_ptr<Mesh> gameMesh[Size];
	std::shared_ptr<Texture> gameTextures[Size];

	//Just doing it simple
	std::shared_ptr<Texture>	boardTextTex;
	std::unique_ptr<Mesh>		boardTextMesh;
	glm::vec3					textTranslate;
	glm::vec3					textScale;
private:
	bool SetTile(glm::vec3 posWS);	
	
	std::unique_ptr<Mesh> grid;
	
	glm::vec3 GetTileCentrePositionWS(const int x, const int y);
	Bounding::Box BB; //Bounding box that will be the board;
//inverse world gives me localspace
	glm::mat4 worldSpace, inverseWorld; //keeping everything in 3d calculations just in case I want to make this 3d instead, with intel SIMD instructions will basically be as good optimized as single instructions anyways -> TODO for a later date
	glm::mat4 boardSpace, inverseBoard; //for optimized grid calculations, top left:(0, 0)	bot right:(gridLengthX, gridLengthY) for fast array access
};

