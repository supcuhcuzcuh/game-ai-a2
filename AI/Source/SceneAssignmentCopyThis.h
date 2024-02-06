#pragma once

#include "Maze.h"
#include "Mtx44.h"
#include "ObjectBase.h"
#include "SceneBase.h"

struct Entity;

class SceneAssignmentCopyThis : public SceneBase, public ObjectBase
{
    Mtx44 _viewMatrix;
    Mtx44 _projectionMatrix;

    Mesh* _quad;
    unsigned _backgroundTexture;
    unsigned _tileLookup[3]; // Change this array to fit how many tiles you got

    std::vector<Entity*> deathRow;
    std::vector<Entity*> birthRow;
    std::vector<Entity*> _entities;
    Maze _maze;
    const int _mazeSize = 20;

    Entity* testEntity;

    template <typename T, typename... ARGS>
    Entity* spawnEntity(ARGS... args)
    {
        Entity* a = new T(args...);
        _entities.push_back(a);
        return a;
    }
    void despawnEntity(Entity* ent);

    void RenderEntity(Entity* ent);
    void RenderMaze();

    unsigned LoadAnyKindOfImage(const char* file);

    bool pathfind(MazePt start, MazePt end, std::vector<MazePt>& output);
    
public:
    void Init() override;
    void Update(double dt) override;
    void Render() override;
    void Exit() override;

    static unsigned testEntityTexture;
    static unsigned testProjectileTexture;
    bool Handle(Message* message) override;
};
