#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <conio.h>
using SHIPREF = size_t;
struct Ship
{
    std::vector<size_t> pos;
};
std::vector<size_t> possibleHorizontalPositions, possibleVerticalPositions;
std::map<size_t, std::vector<SHIPREF>> possiblePositions;
std::map<size_t, SHIPREF> takenPositions;
int numShips{ 0 }, numShipsSunken{0};

constexpr size_t row = 6;
constexpr size_t col = 6;
char board[row][col];

void AddPosition(SHIPREF ship)
{
    const Ship s = *(Ship*)ship;
    for (const auto pos : s.pos)
        takenPositions[pos] = ship;  
}
void DeletePosition(size_t x)
{
    for (const auto val : possiblePositions[x])
    {
        possibleHorizontalPositions.erase(std::remove(possibleHorizontalPositions.begin(), possibleHorizontalPositions.end(), val), possibleHorizontalPositions.end());
        possibleVerticalPositions.erase(std::remove(possibleVerticalPositions.begin(), possibleVerticalPositions.end(), val), possibleVerticalPositions.end());
    }
    possiblePositions.erase(x);
}
SHIPREF Intersect(size_t pos)
{
    const auto it = takenPositions.find(pos);
    const auto end = takenPositions.end();
    if (it != end)
    {
        return it->second;
    }
    return 0;
}
Ship GetRandomHorizontalShip()
{
    const int newRand = std::rand() % possibleHorizontalPositions.size();
    Ship ship = *(Ship*)possibleHorizontalPositions[newRand];
    AddPosition(possibleHorizontalPositions[newRand]);
    for (const auto val : ship.pos)
    {
        DeletePosition(val);
    }
    return ship;
}
Ship GetRandomVerticalShip()
{
    const int newRand = std::rand() % possibleVerticalPositions.size();
    Ship ship = *(Ship*)possibleVerticalPositions[newRand];
    AddPosition(possibleVerticalPositions[newRand]);
    std::cout << "ship pos size: " << ship.pos.size() << std::endl;
    for (const auto val : ship.pos)
    {
        DeletePosition(val);
    }
    return ship;
}
bool Shoot(size_t pos)
{
    SHIPREF shipRef = Intersect(pos);
    if (shipRef)
    {
        Ship* shipPtr = (Ship*)shipRef;
        takenPositions.erase(pos);
        const auto beg = shipPtr->pos.begin();
        const auto end = shipPtr->pos.end();
        shipPtr->pos.erase(std::remove(beg, end, pos), shipPtr->pos.end());
        if (shipPtr->pos.empty())
        { 
            numShips--;
            numShipsSunken++;
        }
        return true;
    }
    return false;
}
bool SetMove(size_t pos)
{
    int x = pos % col;
    int y = pos / col;
    if (Shoot(pos))
    {     
        board[y][x] = 'H';
    }
    else
    {
        if (board[y][x] != 'H')
            board[y][x] = 'M';
    }
    return true;
}
bool GetRandomShip(Ship& ship)
{
    
    if (possibleVerticalPositions.empty() && possibleHorizontalPositions.empty())
        return false;
    if (possibleVerticalPositions.empty() && !possibleHorizontalPositions.empty())
    { 
        ship = GetRandomHorizontalShip();
        return true;
    }
    else
    { 
        ship = GetRandomVerticalShip();
        return true;
    }

    const int rand = std::rand() % 2;
    if (rand)
        ship = GetRandomHorizontalShip();
    else
        ship = GetRandomVerticalShip();
    return true;
};
void MakeBoard3(const int numShips)
{
    do
    {
        Ship ship;
        if (!GetRandomShip(ship))
            break;
        ::numShips++;
        for (size_t i = 0; i < ship.pos.size(); i++)
        {
            int x = ship.pos[i] % col;
            int y = ship.pos[i] / col;
            board[y][x] = 'S';
        }

    } while (::numShips < numShips);
}
void PrintBoard()
{
    for (size_t y = 0; y < row; y++)
    {
        for (size_t x = 0; x < col; x++)
        {
            std::cout << '|' << board[y][x];
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "Number of ships: " << numShips << std::endl;
    std::cout << "Number of Ships sunken " << numShipsSunken << std::endl;
}

int main()
{
    std::srand(std::time(nullptr));
   
  
    std::set<size_t> openTiles;
    std::set<size_t> takenTile;
    for (size_t y = 0; y < row; y++)
    {
        for (size_t x = 0; x < col; x++)
        {
            board[y][x] = ' ';

        }
    }
    //vertical ship insert
    for (size_t y = 1; y < row-1; y++)
    {
        for (size_t x = 0; x < col; x++)
        {
            Ship* ship = new Ship();
            const size_t sX = (y-1) * col + x;
            const size_t sY = y * col + x;
            const size_t sZ = (y + 1) * col + x;    
            ship->pos.push_back(sX);
            ship->pos.push_back(sY);
            ship->pos.push_back(sZ);
            possibleVerticalPositions.push_back((size_t)ship);

            possiblePositions[sX].push_back((size_t)ship);
            possiblePositions[sY].push_back((size_t)ship);
            possiblePositions[sZ].push_back((size_t)ship);
        }
    }
    size_t find = 0;
    //horizontal ship
    for (size_t y = 0; y < row; y++)
    {
        for (size_t x = 1; x < col-1; x++)
        {
            Ship* ship = new Ship();
            find = (size_t)ship;
            const size_t sX = y * col + (x - 1);
            const size_t sY = y * col + x;
            const size_t sZ = y * col + (x + 1);  

            ship->pos.push_back(sX);
            ship->pos.push_back(sY);
            ship->pos.push_back(sZ);
            possibleHorizontalPositions.push_back((size_t)ship);

            possiblePositions[sX].push_back((size_t)ship);
            possiblePositions[sY].push_back((size_t)ship);
            possiblePositions[sZ].push_back((size_t)ship);
        }
    }
    std::cout << "possible pos size: " << possiblePositions.size() << std::endl;
    std::cout << "possible vert size: " << possibleVerticalPositions.size() << std::endl;
    std::cout << "possible hor size: " << possibleHorizontalPositions.size() << std::endl;
   
    MakeBoard3(2);
    PrintBoard();

    std::cout << "possible pos size: " << possiblePositions.size() << std::endl;
    std::cout << "possible vert size: " << possibleVerticalPositions.size() << std::endl;
    std::cout << "possible hor size: " << possibleHorizontalPositions.size() << std::endl;

    do
    {  
        int val{};
        std::cin >> val;
        if (SetMove((int)val))
        {
            system("cls");
            PrintBoard();
            std::cout << "hit a ship" << std::endl;
        }
    } while (true);
}
