#include "precompiled.h"
#include "..\BattleShipsClientOGL\BattleShipsOGLClient.h"
#include "PlayerClient.h"
#include "NetworkEvents.h"
#include <mutex>
#include <ctime>
PlayerClient* gs_pc = nullptr;

PlayerClient::PlayerClient()
    :numRow{ 0 }, numCol{ 0 }, numShots{ 0 }
{
    hWaitBegin = CreateEvent(NULL, NULL, NULL, NULL);
    hWaitUpdate = CreateEvent(NULL, NULL, NULL, NULL);
}

PlayerClient* PlayerClient::Get()
{
    return gs_pc;
}

void PlayerClient::RequestGameBegin()
{
    char buff[DEFAULT_BUFLEN]{};
    int iResult{ 0 };

    Json beginRequest = Json(Json::Object);
    beginRequest.Set("Events", Json::Array);
    beginRequest["Events"].Add(EVENT_BEGIN);
    client.Send(beginRequest.Stringify().c_str());
}

void PlayerClient::GameBegin(const Json& data)
{
    numShots = (int)data["NumShots"];
    numCol = (int)data["NumCol"];
    numRow = (int)data["NumRow"];
}

void PlayerClient::GameUpdate(const Json& data)
{
    numShots = (int)data["NumShots"];
    numHits = (int)data["NumHits"];
    numShipsSunken = (int)data["NumShipsSunken"];

    for (size_t y = 0; y < data["Board"].Size(); y++)
    {
        for (size_t x = 0; x < data["Board"][y].Size(); x++)
        {
            if ((int)data["Board"][y][x] == 'S')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Ship);
            }
            else if ((int)data["Board"][y][x] == 'H')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Hit);
            }
            else if ((int)data["Board"][y][x] == 'M')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Miss);
            }
        }
    }
}

void PlayerClient::GameFinished(const Json& data)
{
    myBoard->gameDrawPositions->clear();
    enemyBoard->gameDrawPositions->clear();
    statusDrawTex.clear();
    for (size_t y = 0; y < data["YourBoard"].Size(); y++)
    {
        for (size_t x = 0; x < data["YourBoard"][y].Size(); x++)
        {
            if ((int)data["YourBoard"][y][x] == 'S')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Ship);
            }
            else if ((int)data["YourBoard"][y][x] == 'H')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Ship);
            }
            else if ((int)data["YourBoard"][y][x] == 'M')
            {
                myBoard->SetTile({ (int)x,(int)y }, Board::Miss);
            }
            if ((int)data["EnemyBoard"][y][x] == 'S')
            {
                enemyBoard->SetTile({ (int)x,(int)y }, Board::Ship);
            }
            else if ((int)data["EnemyBoard"][y][x] == 'H')
            {
                enemyBoard->SetTile({ (int)x,(int)y }, Board::Ship);
            }
            else if ((int)data["EnemyBoard"][y][x] == 'M')
            {
                enemyBoard->SetTile({ (int)x,(int)y }, Board::Miss);
            }
        }
    }
    
    const int enemyShipsSunken = data["EnemyShipsSunken"];
    numShipsSunken = (int)data["YourShipsSunken"];
    
    if (enemyShipsSunken > numShipsSunken)
    {
        statusDrawTex.push_back(statusTextTex[(size_t)GameStatus::Lose]);
    }
    else if (enemyShipsSunken == numShipsSunken)
    {
        statusDrawTex.push_back(statusTextTex[(size_t)GameStatus::Draw]);
    }
    else
    {
        statusDrawTex.push_back(statusTextTex[(size_t)GameStatus::Win]);
    }
    
    //delete[] board;
}

void PlayerClient::GamePlayerShipHit(const Json& data)
{
    iVec2D pos = Convert1Dto2D((int)data["Pos"]);
    const char ch = (int)data["Ch"];
    myBoard->SetTile(pos, Board::Miss);
    //boardReplicate[pos.y][pos.x] = ch;
}

int PlayerClient::Convert2Dto1D(const iVec2D pos)
{
    return (pos.y * numCol) + pos.x;
}

iVec2D PlayerClient::Convert1Dto2D(const int pos)
{
    iVec2D result;
    result.x = pos % numCol;
    result.y = pos / numCol;
    return result;
}

bool PlayerClient::SetTile(const int pos, char ch)
{
    iVec2D pos2D;
    pos2D = Convert1Dto2D(pos);
    //enemyboardReplicate[pos2D.y][pos2D.x] = ch;
    return true;
}

void PlayerClient::SetMove(const int pos, char ch)
{
    if (GetNumShots() > 0)
    {
        iVec2D pos2D = Convert1Dto2D(pos);
        if (ch == 'H')
        {
            enemyBoard->SetTile(pos2D, Board::DrawInfo::Hit);
        }
        else
            enemyBoard->SetTile(pos2D, Board::DrawInfo::Miss);
        /*
        if (enemyboardReplicate[pos2D.y][pos2D.x] == ' ')
            SetTile(pos, ch);
        SetNumShots(GetNumShots() - 1);
        */
    }
}

PlayerClient::ShootStatus PlayerClient::Shoot(iVec2D& pos)
{
    if (numShots <= 0)
    {
        std::cerr << "Player no ammo" << std::endl;
        return ShootStatus::OutOfAmmo;
    }

    //if (!IsValidInput(str, pos))
    //    return ShootStatus::InvalidInput;

    return ShootStatus::OK;
}

void PlayerClient::RequestShoot(const iVec2D pos)
{
    if (GetNumShots() > 0)
    {
        Json response(Json::Object);
        response.Set("Events", Json::Array);
        response.Set("Status", S_OK);
        response["Events"].Add(EVENT_SHOOT);
        response.Set("Event_Shoot", Json::Object);
        response["Event_Shoot"].Set("Pos", Convert2Dto1D(pos));
        client.Send(response.Stringify().c_str());
    }
}

void PlayerClient::RequestGameUpdate()
{
    Json response(Json::Object);
    response.Set("Events", Json::Array);
    response["Events"].Add(EVENT_UPDATE);
    client.Send(response.Stringify().c_str());
}

void PlayerClient::RequestGamePing()
{
    Json request(Json::Object);
    request.Set("Events", Json::Array);
    request["Events"].Add(EVENT_PING);
    ping = std::time(nullptr);
    client.Send(request.Stringify().c_str());
}

void PlayerClient::Connect(const std::string IP)
{
    client.Init(IP);
}

void PlayerClient::Close()
{
    if (listenTH.joinable())
        listenTH.join();
    client.ShutDown();
}

void PlayerClient::PlayerClientListen()
{
    gs_pc = this;
    listenTH = std::thread(PlayerClient::ThreadWrapper);
}


void PlayerClient::ThreadWrapper()
{
    PlayerClient::Get()->OnListen();
}

void PlayerClient::OnListen()
{
    Connect("127.0.0.1");
    RequestGameBegin();
    bool bGame = true;
    do
    {
        char data[DEFAULT_BUFLEN]{};
        int iResult = recv(client.GetSocket(), data, DEFAULT_BUFLEN, 0);
        
        if (iResult > 0)
        {           
            Json request = Json::Parse(data);
            Json response = Json(Json::Object);
            if (request.Contains("Events"))
            {
                
                for (const auto event : request["Events"])
                {
                    std::cout << data << std::endl;
                    switch ((int)event.Value())
                    {
                    case EVENT_BEGIN:
                        GameBegin(request["Event_Begin"]);
                        SetEvent(hWaitBegin);
                        break;
                    case EVENT_UPDATE:
                        GameUpdate(request["Event_Update"]);
                        SetEvent(hWaitUpdate);
                        break;
                    case EVENT_TURN_ACCEPTED:
                        SetMove(request["Event_Turn_Accepted"]["Pos"], (int)request["Event_Turn_Accepted"]["Ch"]);
                        break;
                    case EVENT_FINISHED:
                        GameFinished(request["Event_Finished"]);                        
                        bGame = false;
                        break;
                    case EVENT_PLAYER_SHIP_HIT:
                        GamePlayerShipHit(request["Event_Player_Ship_Hit"]);
                        break;
                    case EVENT_PING:
                        ping = std::time(nullptr) - ping;
                        break;
                    default:
                        break;
                    }
                }
            }
            
        }
        if (iResult < 0)
            return;
    } while (bGame);
    SetEvent(hWaitUpdate); //For best practise to avoid infinite loop in other thread.
}

const size_t PlayerClient::GetNumShots() const
{
    return numShots;
}

void PlayerClient::SetNumShots(const size_t val)
{
    std::mutex mx;
    mx.lock();
    numShots = val;
    mx.unlock();
}

