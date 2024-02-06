#include "World.h"

#include <set>
#include <iostream>

#include "rapidcsv.h"

std::vector<vec2> World::raycastLow(vec2 origin, vec2 target)
{
    std::vector<vec2> tiles;

    int dx = target.first - origin.first;
    int dy = target.second - origin.second;
    int y = origin.second;
    int x_increment = dx > 0 ? 1 : -1;
    int y_increment = 1;
    if (dy < 0)
    {
        y_increment = -1;
        dy = -dy;
    }
    int D = 2*dy - dx;

    for (int x = origin.first; x != target.first; x += x_increment)
    {
        if (D > 0)
        {
            y = y + y_increment;
            D = D + (2 * (dy - dx));
        } else
            D = D + 2*dy;

        tiles.push_back({x,y});
    }

    return tiles;
}

std::vector<vec2> World::raycastHigh(vec2 origin, vec2 target)
{
    std::vector<vec2> tiles;

    int dx = target.first - origin.first;
    int dy = target.second - origin.second;
    int x_increment = 1;
    int y_increment = dy > 0 ? 1 : -1;
    if (dx < 0)
    {
        x_increment = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = origin.first;

    for (int y = origin.second; y != target.second; y += y_increment)
    {
        if (D > 0)
        {
            x = x + x_increment;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;

        tiles.push_back({x,y});
    }

    return tiles;
}

World::World(const std::string& map_file, int w, int h,
             std::vector<WorldTile>& lookup_table)
{
    _width = w; _height = h;
    _largerDimension = _width > _height ? _width : _height;

    delete _worldData;
    _worldData = new WorldTile[_width * _height];

    _entities.clear();
    _locations.clear();

    rapidcsv::Document mapFile(map_file,rapidcsv::LabelParams(-1,-1));
    for (int i = 0; i < _width; ++i)
    {
        auto close = mapFile.GetColumn<int>(i);
        for (int j = 0; j < _height; ++j)
        {
            int cellNumber = close[_height - j - 1];
            int cellNumberSave = cellNumber;
            if (cellNumber < 0 || cellNumber >= lookup_table.size())
            {
                std::cerr << "Attempted To Load Invalid ID | Check if this cell is accounted for in Init()" << std::endl;
                cellNumber = 0; // Default to loading the first tile
            }
            _worldData[coordinateToIndex(vec2{i,j})] = lookup_table[cellNumber];
            _worldData[coordinateToIndex(vec2{i,j})].cellID = cellNumberSave; // Store the Original ID in the cell for getCell()
        }
    }
}

// Converts a vec2 to an int to use in _worldData
int World::coordinateToIndex(vec2 coordinate)
{ return coordinate.first + (_width * coordinate.second); }
int World::coordinateToIndex(vec2f coordinate)
{ return coordinate.first + (_width * coordinate.second); }

// Converts a WorldTile to its index in _worldData
int World::tileToIndex(WorldTile& tile)
{ return &tile - _worldData; }
int World::tileToIndex(WorldTile* tile)
{ return tile - _worldData; }

// Converts int indices to vec2 coordinates
vec2 World::indexToCoordinates(int index)
{
    int yPos = index / _width;
    int xPos = index % _width;
    return {xPos,yPos};
}
vec2 World::tileToCoordinates(WorldTile& tile)
{ return indexToCoordinates(tileToIndex(tile)); }
vec2 World::tileToCoordinates(WorldTile* tile)
{ return indexToCoordinates(tileToIndex(tile)); }

// Returns a pointer to the tile that this entity is located at
WorldTile* World::getEntityLocation(EntityButSmallerSize* ent)
{
    if (_locations.find(ent) == _locations.end()) return nullptr;
    return _locations.at(ent);
}

int World::getWorldSizeLinear()
{ return _width * _height; }
vec2 World::getWorldSize()
{ return {_width,_height}; }
bool World::withinWorld(vec2 position)
{ return !(position.first < 0 || position.second < 0 || position.first >= _width || position.second >= _height); }
bool World::withinWorld(vec2f position)
{ return !(position.first < 0 || position.second < 0 || position.first >= _width || position.second >= _height); }
WorldTile& World::getTile(int index)
{ return _worldData[index]; }
std::vector<int> World::getCells(int cellID)
{
    std::vector<int> result;
    for (int i = 0; i < getWorldSizeLinear(); ++i)
    {
        if (_worldData[i].cellID == cellID)
            result.push_back(i);
    }
    return result;
}

void World::teleportEntity(EntityButSmallerSize* entity, vec2 position)
{
    // Get Entity Tiles
    WorldTile* entTile = _locations[entity];
    int nextIndex = coordinateToIndex(position);
    auto& nextTile = _worldData[nextIndex];

    // Swap Tile Entities
    entTile->entity = nextTile.entity;
    nextTile.entity = entity;
    _locations[entity] = &nextTile;
}

void World::addEntity(EntityButSmallerSize* entity, vec2 position)
{
    _entities.push_back(entity);
    WorldTile& tile = _worldData[coordinateToIndex(position)];
    if (tile.entity)
    {
        std::cerr << "Spawning Entity Over Existing Entity" << std::endl;
    }
    tile.entity = entity;
    _locations.insert(std::make_pair(entity, &tile));
}

#include <limits>

struct AStarCell
{
    vec2 prev {-1,-1};
    float f {std::numeric_limits<float>::max()};
    float g {std::numeric_limits<float>::max()};
    float h {std::numeric_limits<float>::max()};
};

bool World::pathfind(EntityButSmallerSize* entity, vec2 target, Path& output, const std::vector<EntityButSmallerSize*> ignoreList)
{
    if (tileToCoordinates(getEntityLocation(entity)) == target)
    {
        std::cout << "Cannot Pathfind to Self" << std::endl;
        return false;
    }

    std::vector<bool> closed(getWorldSizeLinear(),false);
    std::vector<AStarCell> cellData(getWorldSizeLinear());

    vec2 entityPosition = tileToCoordinates(_locations[entity]);

    vec2 node = entityPosition;
    auto& startingCell = cellData[coordinateToIndex(node)];
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
        closed[coordinateToIndex(node)] = true;

        double f,g,h;

        vec2 upNode = up(node);
        vec2 leftNode = left(node);
        vec2 rightNode = right(node);
        vec2 downNode = down(node);
//        auto leftUpNode = up(left(node));
//        auto upRightNode = right(up(node));
//        auto rightDownNode = down(right(node));
//        auto downLeftNode = left(down(node));
        vec2 nodes[] = {upNode,downNode,rightNode,leftNode};
                        //,upRightNode,leftUpNode,downLeftNode,rightDownNode};
        for (vec2& nextNode : nodes)
        {
            if (withinWorld(nextNode))
            {
                // I forgot what I needed this for
                // if (_worldData[coordinateToIndex(nextNode)].entity)
                //     if (_worldData[coordinateToIndex(nextNode)].entity->modifier != PUSHABLE) continue;

                // I remember now | This makes tiles with entities on them unpassable
                auto* tileEntity = _worldData[coordinateToIndex(nextNode)].entity;
                if (tileEntity)
                {
                    bool ignored = false;
                    for (auto* e : ignoreList)
                    {
                        if (tileEntity == e || tileEntity == entity)
                        {
                            ignored = true;
                            break;
                        }
                    }
                    if (!ignored) continue;
                }

                if (nextNode == target)
                {
                    cellData[coordinateToIndex(nextNode)].prev = node;
                    vec2 a = target;
                    while (a != cellData[coordinateToIndex(a)].prev)
                    {
                        output.emplace_back(a);
                        a = cellData[coordinateToIndex(a)].prev;
                    }
                    output.emplace_back(a);
                    return true;
                }
                if (!closed[coordinateToIndex(nextNode)])
                {
                    // I forgot what I need this for
                    // if (_worldData[coordinateToIndex(nextNode)].entity)
                    //     if (_worldData[coordinateToIndex(nextNode)].entity->modifier != PUSHABLE) continue;

                    // I remember now | This makes tiles with entities on them unpassable
                    if (_worldData[coordinateToIndex(nextNode)].entity && _worldData[coordinateToIndex(nextNode)].entity != entity)
                    {

                        continue;
                    }

                    auto& cell = cellData[coordinateToIndex(nextNode)];
                    g = cellData[coordinateToIndex(node)].g + 1.0;
                    // h = GetHeuristic(V2FMinus(nextNode, target));
                    vec2f diff;
                    diff.first = target.first - nextNode.first;
                    diff.second = target.second - nextNode.second;
                    h = GetHeuristic(diff);
                    f = g+h;

                    if (cell.f  >= std::numeric_limits<float>::max() ||
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

float World::GetHeuristic(vec2f vec)
{
    auto aSQ = vec.first * vec.first;
    auto bSQ = vec.second * vec.second;
    auto cSQ = aSQ + bSQ;
    return std::sqrt(static_cast<float>(cSQ));
}

RaycastInfo World::raycast(vec2 origin, vec2 target, const std::vector<EntityButSmallerSize*> ignoreList)
{
    std::vector<vec2> tiles;
    if (abs(target.second - origin.second) < abs(target.first - origin.first))
    {
        if (origin.first > target.first)
        {
            tiles = raycastLow(target, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastLow(origin, target);
            tiles.emplace_back(target);
        }
    }
    else
    {
        if (origin.second > target.second)
        {
            tiles = raycastHigh(target, origin);
            tiles.emplace_back(origin);
            std::reverse(tiles.begin(),tiles.end());
        }
        else
        {
            tiles = raycastHigh(origin, target);
            tiles.emplace_back(target);
        }
    }
    RaycastInfo info;
    if (tiles.empty())
        return info;
    for (int i = 0; i < tiles.size(); ++i)
    {
        const auto& x = tiles[i];
        if (x.first < 0 || x.second < 0) continue;
        if (x.first >= _width || x.second >= _height) continue;
        auto& tile = _worldData[coordinateToIndex(x)];
        if (tile.entity)
        {
            bool ignored = false;
            for (auto ignore : ignoreList)
            {
                if (tile.entity == ignore)
                {
                    ignored = true;
                    break;
                }
            }
            if (ignored) continue;
            info.hit = &tile;
            info.hitIndex = coordinateToIndex(x);
        }
    }

    return info;
}

int World::up(int i) const
{ return i + _width; }
int World::down(int i) const
{ return i - _width; }
int World::left(int i) const
{ return i - 1; }
int World::right(int i) const
{ return i + 1; }
vec2 World::up(vec2 i) const
{ return {i.first,i.second+1}; }
vec2 World::down(vec2 i) const
{ return {i.first,i.second-1}; }
vec2 World::left(vec2 i) const
{ return {i.first-1,i.second}; }
vec2 World::right(vec2 i) const
{ return {i.first+1,i.second}; }
vec2 World::up(vec2f i) const
{ return {static_cast<int>(i.first),static_cast<int>(i.second+1)}; }
vec2 World::down(vec2f i) const
{ return {static_cast<int>(i.first),static_cast<int>(i.second-1)}; }
vec2 World::right(vec2f i) const
{ return {static_cast<int>(i.first+1),static_cast<int>(i.second)}; }
vec2 World::left(vec2f i) const
{ return {static_cast<int>(i.first-1),static_cast<int>(i.second)}; }
vec2f World::upf(vec2f i) const
{ return {i.first,i.second+1}; }
vec2f World::downf(vec2f i) const
{ return {i.first,i.second-1}; }
vec2f World::leftf(vec2f i) const
{ return {i.first-1,i.second}; }
vec2f World::rightf(vec2f i) const
{ return {i.first+1,i.second}; }
