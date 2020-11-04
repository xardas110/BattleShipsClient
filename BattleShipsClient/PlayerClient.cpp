#include "PlayerClient.h"
#include "NetworkEvents.h"
#include <mutex>
PlayerClient* gs_pc = nullptr;


bool PlayerClient::IsValidInput(const std::string& str, iVec2D& pos)
{
    std::string sStr{ "" };
    int val{ 0 };
    for (size_t i = 0; i < str.length(); i++)
    {

        if (thelp::IsDigit(str[i]))
        {
            int tempVal{ 0 };
            if (thelp::ConvertChToInt(str[i], tempVal))
            {
                val *= 10;
                val += tempVal;
                continue;
            }
            else
            {
                std::cerr << "Digit conversion failed!" << std::endl;
                thelp::This_Thread_Sleep(1000);
                return false;
            }
        }
        if (thelp::IsAlphabetic(str[i]))
        {
            sStr += str[i];
            continue;
        }
        std::cerr << "Contains unrecognized characters!" << std::endl;
        thelp::This_Thread_Sleep(1000);
        return false;
    }

    if (val < 0 || val > numRow)
    {
        std::cerr << "value contains too many rows: " << val << std::endl;
        thelp::This_Thread_Sleep(1000);
        return false;
    }

    if (sStr.length() > 1 or sStr.length() == 0)
    {
        std::cerr << "Too many alphabetic characters" << std::endl;
        thelp::This_Thread_Sleep(1000);
        return false;
    }

    if (sStr[0] < 65 or sStr[0] > 64 + numCol)
    {
        std::cerr << "Alphabetic character out of bounds" << std::endl;
        thelp::This_Thread_Sleep(1000);
        return false;
    }

    pos.x = (int)sStr[0] - 65;
    pos.y = numRow - val;
    return true;

}

PlayerClient::PlayerClient()
    :numRow{ 0 }, numCol{ 0 }, numShots{ 0 }
{
    hWaitUpdate = CreateEvent(
        NULL,
        FALSE,
        FALSE,
        NULL);
    hWaitBegin = CreateEvent(
        NULL,
        FALSE,
        FALSE,
        NULL);
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

    boardReplicate = new char* [numRow];
    enemyboardReplicate = new char* [numRow];
    for (size_t i = 0; i < numRow; i++)
    {
        boardReplicate[i] = new char[numCol];
        enemyboardReplicate[i] = new char[numCol];
    }

    for (size_t y = 0; y < data["Board"].Size(); y++)
    {
        for (size_t x = 0; x < data["Board"][y].Size(); x++)
        {
            boardReplicate[y][x] = (int)data["Board"][y][x];
            enemyboardReplicate[y][x] = ' ';
        }
    }
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
            boardReplicate[y][x] = (int)data["Board"][y][x];
        }
    }
}

void PlayerClient::GameFinished(const Json& data)
{
    for (size_t y = 0; y < data["YourBoard"].Size(); y++)
    {
        for (size_t x = 0; x < data["YourBoard"][y].Size(); x++)
        {
            boardReplicate[y][x] = (int)data["YourBoard"][y][x];
            enemyboardReplicate[y][x] = (int)data["EnemyBoard"][y][x];
        }
    }

    const int enemyShipsSunken = data["EnemyShipsSunken"];
    numShipsSunken = (int)data["YourShipsSunken"];
    const std::string statusText = data["Status"];
    PrintEndScreen(enemyShipsSunken, statusText);

}

void PlayerClient::GamePlayerShipHit(const Json& data)
{
    iVec2D pos = Convert1Dto2D((int)data["Pos"]);
    const char ch = (int)data["Ch"];
    boardReplicate[pos.y][pos.x] = ch;
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
    enemyboardReplicate[pos2D.y][pos2D.x] = ch;
    return true;
}

void PlayerClient::SetMove(const int pos, char ch)
{
    if (GetNumShots() > 0)
    {
        iVec2D pos2D = Convert1Dto2D(pos);
        if (enemyboardReplicate[pos2D.y][pos2D.x] == ' ')
            SetTile(pos, ch);
        SetNumShots(GetNumShots() - 1);
    }
}

PlayerClient::ShootStatus PlayerClient::Shoot(iVec2D& pos)
{
    if (numShots <= 0)
    {
        std::cerr << "Player no ammo" << std::endl;
        thelp::This_Thread_Sleep(2000);
        return ShootStatus::OutOfAmmo;
    }
    std::string str{ "" };
    thelp::AskInputS("", str);

    if (!thelp::ToUpper(str))
        return ShootStatus::FailedToConvertUpper;

    if (!IsValidInput(str, pos))
        return ShootStatus::InvalidInput;

    return ShootStatus::OK;
}

void PlayerClient::RequestShoot()
{
    if (GetNumShots() > 0)
    {
        iVec2D pos;
        while (Shoot(pos) != ShootStatus::OK);
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

void PlayerClient::Connect(const std::string IP)
{
    gs_pc = this;
    client.Init(IP);
}

void PlayerClient::Run()
{
    listenTH = std::thread(PlayerClient::ThreadWrapper);
    RequestGameBegin();
    WaitForSingleObject(hWaitBegin, INFINITE);
    while (GetNumShots() > 0)
    {   
        RequestShoot();
        RequestGameUpdate();
        WaitForSingleObject(hWaitUpdate, INFINITE);           
    }
    client.ShutDown();
    listenTH.join();
}


void PlayerClient::PrintBoard()
{
    system("cls");
    std::cout << "Your board: \t\tEnemy board:" << std::endl;
    for (size_t y = 0, ym = numRow; y < numRow; y++, ym--)
    {
        std::cout << ym << " ";
        for (size_t x = 0; x < numCol; x++)
        {
            std::cout << '|' << boardReplicate[y][x];
        }
        std::cout << "|\t\t" << ym << " ";

        for (size_t x = 0; x < numCol; x++)
        {
            std::cout << '|' << enemyboardReplicate[y][x];
        }
        std::cout << '|' << std::endl;

    }
    WriteLetters();
    std::cout << "Select a tile to shoot at:(i.e A1/a1 or 1A/1a)" << std::endl;
}

void PlayerClient::PrintEndScreen(const int enemyShipsSunken, const std::string& statusText)
{
    system("cls");
    std::cout << "Your board: \t\tEnemy board:" << std::endl;
    for (size_t y = 0, ym = numRow; y < numRow; y++, ym--)
    {
        std::cout << ym << " ";
        for (size_t x = 0; x < numCol; x++)
        {
            std::cout << '|' << boardReplicate[y][x];
        }
        std::cout << "|\t\t" << ym << " ";

        for (size_t x = 0; x < numCol; x++)
        {
            std::cout << '|' << enemyboardReplicate[y][x];
        }
        std::cout << '|' << std::endl;

    }
    std::cout << "You sank: " << numShipsSunken << " ships" << std::endl;
    std::cout << "Enemy sank: " << enemyShipsSunken << " ships" << std::endl;
    std::cout << statusText << std::endl;
}

void PlayerClient::WriteLetters()
{
    const std::string spacing = "  ";
    std::cout << spacing;
    for (int i = 0; i < numCol * 2 + 1; i++)
        std::cout << '-';
    std::cout << "\t\t" + spacing;
    for (int i = 0; i < numCol * 2 + 1; i++)
        std::cout << '-';
    std::cout << std::endl << spacing;
    int currVal{ 65 };
    for (int x = 0; x < numCol; x++)
        std::cout << '|' << (char)currVal++;
    currVal = 65;
    std::cout << "|\t\t  ";
    for (int x = 0; x < numCol; x++)
        std::cout << '|' << (char)currVal++;
    std::cout << '|' << std::endl;
    std::cout << "Num Shots: " << numShots << " - Num Hits: " << numHits << " - Ships sunken: " << numShipsSunken << std::endl;
}

void PlayerClient::ThreadWrapper()
{
    PlayerClient::Get()->OnListen();
}

void PlayerClient::OnListen()
{
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
                    switch ((int)event.Value())
                    {
                    case EVENT_BEGIN:
                        GameBegin(request["Event_Begin"]);
                        PrintBoard();
                        SetEvent(hWaitBegin);
                        break;
                    case EVENT_UPDATE:
                        GameUpdate(request["Event_Update"]);
                        PrintBoard();
                        SetEvent(hWaitUpdate);
                        break;
                    case EVENT_TURN_ACCEPTED:
                        SetMove(request["Event_Turn_Accepted"]["Pos"], (int)request["Event_Turn_Accepted"]["Ch"]);
                        PrintBoard();
                        break;
                    case EVENT_FINISHED:
                        GameFinished(request["Event_Finished"]);                        
                        bGame = false;
                        break;
                    case EVENT_PLAYER_SHIP_HIT:
                        GamePlayerShipHit(request["Event_Player_Ship_Hit"]);
                        PrintBoard();
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

