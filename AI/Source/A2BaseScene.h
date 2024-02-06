#pragma once
#include "EntityButSmallerSize.h"
#include "SceneBase.h"
#include "World.h"

#define DEBUGLOG(msg) std::cout << "[INFO]\t" << msg << std::endl
#define UnpackVector(vec) vec.x,vec.y,vec.z

using Turn = size_t;
class A2BaseScene : public SceneBase
{
protected:
    int _windowWidth;
    int _windowHeight;
    const int _cameraViewRange = 20;
    float _aspectRatio;

    Mesh* _square; // Used by all Sprites

    World _world; // The World containing the tiles and other entities

    // Turn System Variables
    Turn _currentTurn = 0;
    Turn _timer;
    bool _playerTurn = false;
    bool _gameOver = false;
    const ActionPoints _basePlayerAP;

    // Sub-World for Entities with a turn
    std::vector<EntityButSmallerSize*> _mobs;
    EntityButSmallerSize* _player = nullptr; // The Player Entity;

    void RenderWorld();
    void RenderWorldWithFog();
    void RenderCourseLine(const std::vector<vec2>& course);
    void MoveCamera(const Vector3& offset);

    vec2 GetMousePosition();
    unsigned LoadAnyKindOfImageAtAllFromTheImagesFolderInTheProjectFiles(const char* file);

    bool DoNextTurn();
    bool CycleTurn();
    void TurnUpdate();

    virtual bool PlayerLostDecision() = 0;
    virtual void OnPlayerLost() = 0;
    virtual void PlayerDecision() = 0;

    virtual void MapUpdate() = 0;
    virtual void OnMobTouchedPlayer() = 0;

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;

    A2BaseScene();
};
