#pragma once
#include "A2BaseScene.h"

class A2SceneLevel2 : public A2BaseScene
{
protected:
    bool PlayerLostDecision() override;
    void OnPlayerLost() override;
    void PlayerDecision() override;
    void MapUpdate() override;
    void OnMobTouchedPlayer() override;

    unsigned grassTexture = 0;
    unsigned wallTexture = 0;
    unsigned enemyTexture = 0;
    unsigned playerTexture = 0;
    unsigned exitTexture = 0;
  
    std::vector<size_t> potentialCellsToDisappear;
    vec2 level1EndPoint = {};
    EntityButSmallerSize wall;

public:
    virtual void Init() override;
    virtual void Update(double deltaTime) override;
    virtual void Render() override;
    virtual void Exit() override;

    A2SceneLevel2();
};
