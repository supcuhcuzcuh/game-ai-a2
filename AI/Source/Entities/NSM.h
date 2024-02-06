#pragma once

struct Entity;
struct NState;

class NStateMachine
{
protected:
    Entity* _owner;
    NState* _currentState;
    
public:
    NStateMachine(Entity* o);

    virtual void update(double deltaTime);
    void switchState(NState* state);

    Entity* getOwner();
};
