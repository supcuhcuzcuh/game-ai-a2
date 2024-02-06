#include "aMaze.hpp"
#include "rapidcsv.h"

#include <iostream>
#include <algorithm>
#include <cmath>

void MazeA::init(int w, int h, unsigned int base_texture)
{
    _width = w; _height = h;
    _larger = _width > _height ? _width : _height;

    delete _mazeData;
    _mazeData = new MazeTile[_width * _height];

    auto count = getTileCount();
    if (!base_texture) return;
    for (int i = 0; i < count; ++i)
        _mazeData[i].texture = base_texture;
}

void MazeA::init(const std::string& csvFile, int w, int h, std::vector<MazeTile>& lookupTable)
{
    _width = w; _height = h;
    _larger = _width > _height ? _width : _height;

    delete _mazeData;
    _mazeData = new MazeTile[_width * _height];

    _entities.clear();
    _entityLocations.clear();

    for (int i = 0; i < lookupTable.size(); ++i)
        lookupTable[i].cell_id = i;

    rapidcsv::Document mapFile(csvFile,rapidcsv::LabelParams(-1,-1));
    for (int i = 0; i < _width; ++i)
    {
        auto close = mapFile.GetColumn<int>(i);
        for (int j = 0; j < _height; ++j)
        {
            int cellNumber = close[_height - j - 1];
            int cellNumberBackup = cellNumber;
            if (cellNumber < 0 || cellNumber >= lookupTable.size())
            {
                std::cerr << "Attempted To Place Invalid Tile" << std::endl;
                cellNumber = 0;
            }
            _mazeData[coordToIndex({i,j})] = lookupTable[cellNumber];
            _mazeData[coordToIndex({i,j})].cell_id = cellNumberBackup;
        }
    }
}

int MazeA::FindNearestEmpty(int start)
{
    int nearest = 0;
    const int grid_boundary = getTileCount();

    int curr = start;
    int doubleDepth = _larger * 2;

    for (int i = 0; i < _larger; ++i)
        curr = up(curr);
    if (curr >= grid_boundary || curr < 0)
        nearest = -1;
    else
        nearest = curr;
    if (isTileEmpty(nearest)) return nearest;
    for (int i = 0; i < _larger; ++i)
    {
        curr = right(curr); // Travel Index
        if (curr >= grid_boundary || curr < 0) // Check if Index Goes Out of Bounds
        {
            nearest = -1; // Set Found Entity to nullptr
            continue; // Skip Candidate Checks | There are no Candidates | Continue instead of Break to keep continuity of Spiral Search
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = down(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = left(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < doubleDepth; ++i)
    {
        curr = up(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }
    for (int i = 0; i < _larger; ++i)
    {
        curr = right(curr);
        if (curr >= grid_boundary || curr < 0)
        {
            nearest = -1;
            continue;
        }
        else
            nearest = curr;
        if (isTileEmpty(nearest)) return nearest;
    }

    return nearest;
}

int MazeA::up(int i) const
{ return i + _width; }
int MazeA::down(int i) const
{ return i - _width; }
int MazeA::left(int i) const
{ return i - 1; }
int MazeA::right(int i) const
{ return i + 1; }
vec2 MazeA::up(vec2 i) const
{ return {i.x,i.y+1}; }
vec2 MazeA::down(vec2 i) const
{ return {i.x,i.y-1}; }
vec2 MazeA::left(vec2 i) const
{ return {i.x-1,i.y}; }
vec2 MazeA::right(vec2 i) const
{ return {i.x+1,i.y}; }

RaycastHitInfo MazeA::raycast(vec2 origin, vec2 end, EntityButSmallerSize *ignore)
{
    std::vector<vec2> tiles;
    if (abs(end.y - origin.y) < abs(end.x - origin.x))
    {
        if (origin.x > end.x)
        {
            tiles = raycastLow(end, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastLow(origin, end);
            tiles.emplace_back(end);
        }
    }
    else
    {
        if (origin.y > end.y)
        {
            tiles = raycastHigh(end, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastHigh(origin, end);
            tiles.emplace_back(end);
        }
    }
    RaycastHitInfo info;
    if (tiles.empty())
        return info;
    bool firstHitHit = false;
    for (int i = 0; i < tiles.size(); ++i)
    {
        const auto& x = tiles[i];
        if (x.x < 0 || x.y < 0) continue;
        if (x.x >= _width || x.y >= _height) continue;
        auto& tile = _mazeData[coordToIndex(x)];
        info.hits.emplace_back(
                    tile,
                    x
                );
        if (tile.entity && tile.entity != ignore && !firstHitHit)
        {
            firstHitHit = true;
            info.firstHit = &tile;
            info.firstHitIndex = i;
        }
    }

    return info;
}

std::vector<vec2> MazeA::raycastLow(vec2 origin, vec2 end)
{
    std::vector<vec2> tiles;

    int dx = end.x - origin.x;
    int dy = end.y - origin.y;
    int y = origin.y;
    int x_increment = dx > 0 ? 1 : -1;
    int y_increment = 1;
    if (dy < 0)
    {
        y_increment = -1;
        dy = -dy;
    }
    int D = 2*dy - dx;

    for (int x = origin.x; x != end.x; x += x_increment)
    {
        if (D > 0)
        {
            y = y + y_increment;
            D = D + (2 * (dy - dx));
        } else
            D = D + 2*dy;

        tiles.emplace_back(x,y);
    }

    return tiles;
}

std::vector<vec2> MazeA::raycastHigh(vec2 origin, vec2 end)
{
    std::vector<vec2> tiles;

    int dx = end.x - origin.x;
    int dy = end.y - origin.y;
    int x_increment = 1;
    int y_increment = dy > 0 ? 1 : -1;
    if (dx < 0)
    {
        x_increment = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = origin.x;

    for (int y = origin.y; y != end.y; y += y_increment)
    {
        if (D > 0)
        {
            x = x + x_increment;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;

        tiles.emplace_back(x,y);
    }

    return tiles;
}

void MazeA::moveEntity(EntityButSmallerSize* ent, vec2 diff)
{
    // Get Entity
    MazeTile* entTile = _entityLocations[ent];
    vec2 entityPosition = indexToCoord(tileToIndex(entTile));
    vec2 endPos = entityPosition;
    endPos.x += diff.x;
    endPos.y += diff.y;
    auto rayinfo = raycast(entityPosition, endPos, ent);

    if (rayinfo.hits.empty())
        return;
    if (rayinfo.firstHit) // Check if the raycast hit something
    {
        if (rayinfo.firstHit->entity != ent)
        {
            if (rayinfo.firstHitIndex > 0) // Check if the first hit is not next to the entity
            {
                entityPosition = rayinfo.hits[rayinfo.firstHitIndex-1].second;
                int nextIndex = coordToIndex(entityPosition);
                auto& nextTile = _mazeData[nextIndex];

                if (nextTile.entity)
                        return;

                // Swap Tile Entities;
                entTile->entity = nextTile.entity;
                nextTile.entity = ent;
                _entityLocations[ent] = &nextTile;
                return;
            }
        }
    }

    entityPosition = (rayinfo.hits.end()-1)->second;
    int nextIndex = coordToIndex(entityPosition);
    auto& nextTile = _mazeData[nextIndex];

    if (nextTile.entity)
            return;

    // Swap Tile Entities;
    entTile->entity = nextTile.entity;
    nextTile.entity = ent;
    _entityLocations[ent] = &nextTile;
}

void MazeA::teleportEntity(EntityButSmallerSize *ent, vec2 end)
{
    // Get Entity Tiles
    MazeTile* entTile = _entityLocations[ent];
    int nextIndex = coordToIndex(end);
    auto& nextTile = _mazeData[nextIndex];

    // Swap Tile Entities
    entTile->entity = nextTile.entity;
    nextTile.entity = ent;
    _entityLocations[ent] = &nextTile;
}

#include <set>
#define NOMINMAX // Prevent windows.h min and max macros
#include <limits>

struct AStarCell
{
    vec2 prev {-1,-1};
    float f {std::numeric_limits<float>::max()};
    float g {std::numeric_limits<float>::max()};
    float h {std::numeric_limits<float>::max()};
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
// Following https://www.geeksforgeeks.org/a-search-algorithm/
bool MazeA::pathfind(EntityButSmallerSize* ent, vec2 end, std::vector<vec2>& output)
{
    if (getEntityPosition(ent) == end)
    {
        std::cout << "Cannot Pathfind to Self" << std::endl;
        return false;
    }

    std::vector<bool> closed(getTileCount(),false);
    std::vector<AStarCell> cellData(getTileCount());

    vec2 entityPosition = indexToCoord(tileToIndex(_entityLocations[ent]));

    vec2 node = entityPosition;
    auto& startingCell = cellData[coordToIndex(node)];
    startingCell.f = 0.f;
    startingCell.g = 0.f;
    startingCell.h = 0.f;
    startingCell.prev = node;

    std::set<std::pair<double,vec2>> open;
    open.insert(std::make_pair(0.0, node));

    while (!open.empty())
    {
        auto p = *open.begin();
        open.erase(open.begin());

        node = p.second;
        closed[coordToIndex(node)] = true;

        double f,g,h;

        auto upNode = up(node);
        auto leftNode = left(node);
        auto rightNode = right(node);
        auto downNode = down(node);
//        auto leftUpNode = up(left(node));
//        auto upRightNode = right(up(node));
//        auto rightDownNode = down(right(node));
//        auto downLeftNode = left(down(node));
        vec2 nodes[] = {upNode,downNode,rightNode,leftNode};
                        //,upRightNode,leftUpNode,downLeftNode,rightDownNode};
        for (auto& nextNode : nodes)
        {
            if (withinBounds(nextNode))
            {
                if (_mazeData[coordToIndex(nextNode)].entity) continue;
                if (V2E(nextNode, end))
                {
                    cellData[coordToIndex(nextNode)].prev = node;
                    vec2 a = end;
                    while (!V2E(a, cellData[coordToIndex(a)].prev))
                    {
                        output.emplace_back(a);
                        a = cellData[coordToIndex(a)].prev;
                    }
                    output.emplace_back(a);
                    return true;
                }
                if (!closed[coordToIndex(nextNode)])
                {
                    if (_mazeData[coordToIndex(nextNode)].entity) continue;

                    auto& cell = cellData[coordToIndex(nextNode)];
                    g = cellData[coordToIndex(node)].g + 1.0;
                    h = GetHeuristic(V2FMinus(nextNode.fromInt(), end.fromInt()));
                    f = g+h;

                    if (cell.f  == std::numeric_limits<float>::max() ||
                            cell.f > f)
                    {
                        open.insert(std::make_pair(f, nextNode));

                        cell.f = f;
                        cell.g = g;
                        cell.h = h;
                        cell.prev = node;
                    }
                }
            }
        }
    }

    return false;
}
#pragma clang diagnostic pop

float MazeA::GetHeuristic(vec2f vec)
{
    auto aSQ = vec.x * vec.x;
    auto bSQ = vec.y * vec.y;
    auto cSQ = aSQ + bSQ;
    return std::sqrt(static_cast<float>(cSQ));
}

bool MazeA::withinBounds(vec2 pos)
{
    // return true if within the bounds of the maze
    return !(pos.x < 0 || pos.y < 0 || pos.x >= _width || pos.y >= _height);
}

bool MazeA::V2E(vec2 a, vec2 b)
{ return a.x == b.x && a.y == b.y; }
vec2 MazeA::V2Minus(vec2 a, vec2 b)
{ return {b.x - a.x, b.y - a.y}; }

vec2f MazeA::V2FMinus(vec2f a, vec2f b)
{ return {static_cast<float>(b.x) - static_cast<float>(a.x), static_cast<float>(b.y) - static_cast<float>(1)}; }

void MazeA::print_map()
{
    for (int i = 0; i < _width; ++i)
    {
        for (int j = 0; j < _height; ++j)
        {
            const auto& e = _mazeData[coordToIndex({i,j})];
            int no = 0;
            if (e.entity)
                no = e.texture;
            std::cout << no << '|';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

MazeA::~MazeA()
{
    for (auto ent : _entities)
        delete ent;
    _entities.clear();
    _entityLocations.clear();
    delete _mazeData; // Delete World
}

std::vector<size_t> MazeA::getCells(int cellType)
{
    std::vector<size_t> cells;

    for (size_t i = 0; i < getTileCount(); ++i)
    {
        if (_mazeData[i].cell_id == cellType)
            cells.emplace_back(i);
    }

    return cells;
}
