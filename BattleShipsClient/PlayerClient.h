#pragma once
#include "ClientTCP.h"
#include "Json.h"
#include "TerminalHelpers.h"
#include "Vec.h"
#include <thread>

class PlayerClient
{
	std::thread listenTH;
	HANDLE hWaitUpdate, hWaitBegin;
	char** boardReplicate{ nullptr };
	char** enemyboardReplicate{ nullptr };
	Client client;
	size_t numShots{}, numHits{};
	size_t numRow{}, numCol{}, numShipsSunken{};
	bool IsValidInput(const std::string& str, iVec2D& pos);
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

	int Convert2Dto1D(const iVec2D pos);
	iVec2D Convert1Dto2D(const int pos);
	bool SetTile(const int pos, char ch);
	void SetMove(const int pos, char ch);
	ShootStatus Shoot(iVec2D& pos);
	void PrintBoard();
	void PrintEndScreen(const int enemyShipsSunken, const std::string& statusText);
	void WriteLetters();
	static void ThreadWrapper();
	virtual void OnListen();
	const size_t GetNumShots() const;
	void SetNumShots(const size_t val);
public:
	PlayerClient();
	static PlayerClient* Get();
	void RequestGameBegin();
	void RequestShoot();
	void RequestGameUpdate();
	void Connect(const std::string IP);
	void Run();
};

