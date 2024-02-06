#pragma once

class NStateMachine;

struct NState
{
    NStateMachine* sm;
    
    virtual void update(double deltaTime) = 0;
    virtual void enter() = 0;
    virtual void exit() = 0;

    NState(NStateMachine* machine);
    virtual ~NState() = default;
};
