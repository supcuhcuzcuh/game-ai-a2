#pragma once

#include "../Entity.h"

class GenericProjectileEntity : public Entity
{
    double _timer = 0;
    double _deathTimer = 0;
public:
    MazePt direction{};
    float speed = 0;
    float lifeTime = 0;

    GenericProjectileEntity();

    bool Handle(Message* message) override;
    void update(double deltaTime) override;
};
