#pragma once
#include "ClientTCP.h"
#include "Json.h"
#include "TerminalHelpers.h"
#include "Vec.h"
#include "Board.h"
#include <thread>
#include <ctime>

class PlayerClient
{
	std::thread listenTH;
	
	Client client;
	time_t ping{};
	size_t numShots{}, numHits{};
	
	//bool IsValidInput(const std::string& str, iVec2D& pos);
	enum ShootStatus
	{
		OutOfAmmo,
		OK,
		FailedToConvertUpper,
		InvalidInput
	};
	
	void GameBegin(const Json& data);
	void GameUpdate(const Json& data);
	void GameFinished(const Json& data);
	void GamePlayerShipHit(const Json& data);


	bool SetTile(const int pos, char ch);
	void SetMove(const int pos, char ch);

	ShootStatus Shoot(iVec2D& pos);
	void WriteLetters();
	static void ThreadWrapper();
	virtual void OnListen();
	const size_t GetNumShots() const;
	void SetNumShots(const size_t val);
public:
	enum class GameStatus
	{
		Win, Lose, Draw, Size
	};
	int Convert2Dto1D(const iVec2D pos);
	iVec2D Convert1Dto2D(const int pos);
	size_t numRow{}, numCol{}, numShipsSunken{};
	HANDLE hWaitUpdate, hWaitBegin;
	std::unique_ptr<Board> enemyBoard, myBoard;
	PlayerClient();
	static PlayerClient* Get();
	void RequestGameBegin();
	void RequestShoot(const iVec2D pos);
	void RequestGameUpdate();
	void RequestGamePing();
	void Connect(const std::string IP);
	void Close();
	void PlayerClientListen();


	std::unique_ptr<Mesh> statusTextMesh;
	std::shared_ptr<Texture> statusTextTex[(size_t)GameStatus::Size];
	std::vector<std::shared_ptr<Texture>> statusDrawTex;
	glm::vec3 statusTextTranslate, statusTextScale;
};

