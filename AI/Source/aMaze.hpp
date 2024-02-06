#ifndef DX1220_MAZE_HPP
#define DX1220_MAZE_HPP

#include "EntityButSmallerSize.h"

#include <unordered_map>
#include <vector>
#include <cmath>
#include <string>

struct MazeTile
{
    EntityButSmallerSize* entity = nullptr;
    unsigned texture = 0;
    int cost = 1;
    int cell_id;
};

struct EnemySpawnData
{
    unsigned texture;
    int action_points;
    int spawnIndex;
    double base_speed;
};

struct RaycastHitInfo
{
    typedef std::pair<MazeTile&,vec2> result;
    std::vector<result> hits;
    MazeTile* firstHit = nullptr;
    int firstHitIndex = 0;
};

class MazeA
{
    int _width = 0;
    int _height = 0;
    int _larger = 0;

    MazeTile* _mazeData = nullptr;
    std::vector<EntityButSmallerSize*> _entities;
    std::unordered_map<EntityButSmallerSize*,MazeTile*> _entityLocations;

    int FindNearestEmpty(int start);
    float GetHeuristic(vec2f vec);
    bool V2E(vec2 a, vec2 b);
    vec2 V2Minus(vec2 a, vec2 b);
    vec2f V2FMinus(vec2f a, vec2f b);

    std::vector<vec2> raycastLow(vec2 origin, vec2 end);
    std::vector<vec2> raycastHigh(vec2 origin, vec2 end);

public:

    ~MazeA();

    bool withinBounds(vec2 pos);
    int up(int i) const;
    vec2 up(vec2 i) const;
    int down(int i) const;
    vec2 down(vec2 i) const;
    int left(int i) const;
    vec2 left(vec2 i) const;
    int right(int i) const;
    vec2 right(vec2 i) const;

    void init(int w, int h, unsigned base_texture = 0);
    void init(const std::string& csvFile, int w, int h, std::vector<MazeTile>& lookupTable);

    constexpr const std::vector<EntityButSmallerSize*>& getEntities()
    { return _entities; }
    constexpr vec2 getSize() const
    { return {_width, _height}; }
    constexpr int getTileCount() const
    { return _width * _height; }
    constexpr vec2 indexToCoord(int index)
    {
        int yPos = index / _width;
        int xPos = index % _width;
        return {xPos,yPos};
    }
    constexpr int coordToIndex(vec2 coord)
    { return static_cast<int>(coord.x) + (_width * static_cast<int>(coord.y)); }
    constexpr int tileToIndex(const MazeTile& tile)
    { return &tile - _mazeData; }
    constexpr int tileToIndex(MazeTile* tile)
    { return tile - _mazeData; }
    constexpr vec2 getEntityPosition(EntityButSmallerSize* ent)
    { return indexToCoord(tileToIndex(_entityLocations[ent])); }
    constexpr MazeTile& operator[](int index)
    { return _mazeData[index]; }
    EntityButSmallerSize* spawnEntity(vec2 pos = {})
    {
        auto* e = new EntityButSmallerSize;
        _entities.emplace_back(e);
        auto& tile = _mazeData[coordToIndex(pos)];
        tile.entity = e;
        _entityLocations.insert({e, &tile});
        return e;
    }
    MazeTile* findEntityTile(EntityButSmallerSize* ent)
    { return _entityLocations[ent]; }
    constexpr bool isTileEmpty(int index)
    { return _mazeData[index].entity != nullptr; }

    RaycastHitInfo raycast(vec2 origin, vec2 end, EntityButSmallerSize*ignore);
    bool pathfind(EntityButSmallerSize* ent, vec2 end, std::vector<vec2>& output);

    void moveEntity(EntityButSmallerSize* ent, vec2 diff);
    void teleportEntity(EntityButSmallerSize* ent, vec2 end);

    static double length(vec2 vec)
    {
        return std::sqrt(
                (vec.x) * (vec.x) +
                (vec.y) * (vec.y)
        );
    }
    
    std::vector<size_t> getCells(int cellType);

    void print_map();
};


#endif //DX1220_MAZE_HPP
