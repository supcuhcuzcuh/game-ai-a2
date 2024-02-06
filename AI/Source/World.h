#pragma once
#include <map>
#include <string>

#include "EntityButSmallerSize.h"

struct WorldTile
{
    EntityButSmallerSize* entity = nullptr;
    unsigned texture = 0; // Texture of the Ground with no Entity on it
    ActionPoints cost = 1; // The AP Cost of Traversal
    int cellID = -1;
};

struct RaycastInfo
{
    WorldTile* hit = nullptr;
    int hitIndex = -1;
};

class World
{
    int _width;
    int _height;
    int _largerDimension;

    WorldTile* _worldData = nullptr;

    std::vector<EntityButSmallerSize*> _entities;
    std::map<EntityButSmallerSize*, WorldTile*> _locations;

    std::vector<vec2> raycastLow(vec2 origin, vec2 target);
    std::vector<vec2> raycastHigh(vec2 origin, vec2 target);

public:
    World() = default;
    World(const std::string& map_file, int w, int h,
        std::vector<WorldTile>& lookup_table);

    // Conversion Functions
    int coordinateToIndex(vec2 coordinate);
    int coordinateToIndex(vec2f coordinate);
    int tileToIndex(WorldTile& tile);
    int tileToIndex(WorldTile* tile);

    vec2 indexToCoordinates(int index);
    vec2 tileToCoordinates(WorldTile& tile);
    vec2 tileToCoordinates(WorldTile* tile);

    WorldTile* getEntityLocation(EntityButSmallerSize* ent);

    // Functions For Altering Coordinates
    int up(int i) const;
    int down(int i) const;
    int left(int i) const;
    int right(int i) const;
    vec2 up(vec2 i) const;
    vec2 down(vec2 i) const;
    vec2 left(vec2 i) const;
    vec2 right(vec2 i) const;
    vec2 up(vec2f i) const;
    vec2 down(vec2f i) const;
    vec2 left(vec2f i) const;
    vec2 right(vec2f i) const;
    vec2f upf(vec2f i) const;
    vec2f downf(vec2f i) const;
    vec2f leftf(vec2f i) const;
    vec2f rightf(vec2f i) const;

    // Statistics
    int getWorldSizeLinear();
    vec2 getWorldSize();
    bool withinWorld(vec2 position);
    bool withinWorld(vec2f position);
    WorldTile& getTile(int index);
    std::vector<int> getCells(int cellID);

    // Entity Moving Functions
    void teleportEntity(EntityButSmallerSize* entity, vec2 position);
    void addEntity(EntityButSmallerSize* entity, vec2 position = {});

    // Algorithms
    bool pathfind(EntityButSmallerSize* entity, vec2 target, Path& output, const std::vector<EntityButSmallerSize*> ignoreList = {});
    RaycastInfo raycast(vec2 origin, vec2 target, const std::vector<EntityButSmallerSize*> ignoreList = {});
    float GetHeuristic(vec2f vec);
};
