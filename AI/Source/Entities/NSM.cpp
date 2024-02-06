#include "NSM.h"

#include "NState.h"

NStateMachine::NStateMachine(Entity* o) : _owner(o) { }

void NStateMachine::update(double deltaTime)
{ _currentState->update(deltaTime); }

void NStateMachine::switchState(NState* state)
{
    // Switching States
    if (_currentState)
        _currentState->exit();
    _currentState = state;
    _currentState->enter();
}

Entity* NStateMachine::getOwner()
{
    return _owner;
}
