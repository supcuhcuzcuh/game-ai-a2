#pragma once

#include "NSM.h"
#include "NState.h"
#include "../Entity.h"
#include "../Maze.h"
#include "../Tracker.h"

struct GenericWanderState : public NState
{
    Tracker tracker;

    GenericWanderState(NStateMachine* machine);
    
    void update(double deltaTime) override;
    void enter() override;
    void exit() override;
};

struct GenericSpawnState : public NState
{
    GenericSpawnState(NStateMachine* machine);

    void update(double deltaTime) override;
    void enter() override;
    void exit() override;
};

struct GenericShootState : public NState
{
    GenericShootState(NStateMachine* machine);

    void update(double deltaTime) override;
    void enter() override;
    void exit() override;
};

class GenericStateMachine : public NStateMachine
{
public:
    GenericWanderState wander_state{this};
    GenericSpawnState spawn_state{this};
    GenericShootState shoot_state{this};
    
    GenericStateMachine(Entity* o);
};

class GenericWanderEntity : public Entity
{
public:
    GenericStateMachine* state_machine;
    
    GenericWanderEntity();
    
    void update(double deltaTime) override;
    bool Handle(Message* message) override;
};
