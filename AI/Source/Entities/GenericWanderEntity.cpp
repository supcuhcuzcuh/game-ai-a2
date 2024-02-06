#include "GenericWanderEntity.h"

#include <iostream>
#include <random>

#include "GenericProjectileEntity.h"
#include "../Messages.h"
#include "../PostOffice.h"
#include "../SceneAssignmentCopyThis.h"

GenericStateMachine::GenericStateMachine(Entity* o) : NStateMachine(o)
{
    _currentState = &wander_state;
    _currentState->enter();
}

GenericWanderState::GenericWanderState(NStateMachine* machine) : NState(machine)
{
    tracker = Tracker(&sm->getOwner()->position,0.25);
}

void GenericWanderState::update(double deltaTime)
{
    auto* gsm = static_cast<GenericStateMachine*>(sm);
    bool status = tracker.update(deltaTime);
    if (!status)
        gsm->switchState(&gsm->spawn_state);
}

void GenericWanderState::enter()
{
    // Sending a Message using QuickSend
    GetRandomEmptyPositionMessage* message = PostOffice::GetInstance()->QuickSend<GetRandomEmptyPositionMessage>("MainScene");
    if (!message) // If the message fails to send, switch to some other state
        std::cerr << "Failed to Send Message" << std::endl;
    
    // Sending a Message using the normal provided Send function
    PathfindMessage* pathfinder = new PathfindMessage;
    pathfinder->start = sm->getOwner()->position;
    pathfinder->end = {message->x,message->y};
    bool status = PostOffice::GetInstance()->Send("MainScene",pathfinder);
    if (!status)
        std::cerr << "Failed to Send Message" << std::endl;
    tracker.loadRoute(pathfinder->route);
    tracker.reset();

    // Delete our messages, prevent Memory Leak
    delete message;
    delete pathfinder;
}

void GenericWanderState::exit() { }

GenericSpawnState::GenericSpawnState(NStateMachine* machine) : NState(machine) { }

void GenericSpawnState::update(double deltaTime)
{
}

void GenericSpawnState::enter()
{
    auto* gsm = static_cast<GenericStateMachine*>(sm);
    gsm->switchState(&gsm->shoot_state); // Switch State back to Wander Immediately

    // Spawn an Entity
    auto* spawn_entity_message = PostOffice::GetInstance()->QuickSend<SpawnEntityMessage>("MainScene",new GenericWanderEntity(),SceneAssignmentCopyThis::testEntityTexture);
    // Not necessary but you can check for Post Office Errors by checking if the returned message is nullptr
    // __FILE__ and __LINE__ inserts the file and line where it was called
    if (!spawn_entity_message) std::cerr << "Post Office Failure | " << __LINE__ << " | " << __FILE__ << std::endl;
    delete spawn_entity_message;

    // Killing an Entity (In this case is this Entity)
    auto* kill_entity_message = PostOffice::GetInstance()->QuickSend<KillEntityMessage>("MainScene",sm->getOwner());
    if (!kill_entity_message) std::cerr << "Post Office Failure | " << __LINE__ << " | " << __FILE__ << std::endl;
    delete kill_entity_message;
}

void GenericSpawnState::exit()
{
}

GenericWanderEntity::GenericWanderEntity()
{
    state_machine = new GenericStateMachine(this);
}

void GenericWanderEntity::update(double deltaTime)
{
    state_machine->update(deltaTime);
}

bool GenericWanderEntity::Handle(Message* message)
{
    return false;
}

GenericShootState::GenericShootState(NStateMachine* machine) : NState(machine) { }

void GenericShootState::update(double deltaTime)
{
}

void GenericShootState::enter()
{
    MazePt dir;
    auto* message = PostOffice::GetInstance()->QuickSend<FindNearestEntity>("MainScene", sm->getOwner(), "NOT_A_PROJECTILE");
    if (!message) std::cerr << "Post Office Failure | " << __LINE__ << " | " << __FILE__ << std::endl;
    if (!message->nearest)
    {
        std::cerr << "No Nearest Entity | " << __LINE__ << " | " << __FILE__ << std::endl;
        dir = {1,1};
    }
    else
    {
        dir.x = message->nearest->position.x - sm->getOwner()->position.x;
        dir.y = message->nearest->position.y - sm->getOwner()->position.y;
        dir.x = static_cast<int>(static_cast<double>(dir.x) / std::sqrt(dir.x*dir.x+dir.y*dir.y));
        dir.y = static_cast<int>(static_cast<double>(dir.y) / std::sqrt(dir.x*dir.x+dir.y*dir.y));
    }
    delete message;
    
    GenericProjectileEntity* otherEntity = new GenericProjectileEntity();
    auto* spawnMessage = PostOffice::GetInstance()->QuickSend<SpawnEntityMessage>("MainScene", otherEntity, SceneAssignmentCopyThis::testProjectileTexture);
    otherEntity->position = sm->getOwner()->position; // yes
    otherEntity->direction = dir; // Move in this direction
    otherEntity->speed = .5f; // Move every 0.1 seconds
    otherEntity->lifeTime = 4; // Live for 1 Second
    delete spawnMessage;

    auto* gsm = static_cast<GenericStateMachine*>(sm);
    gsm->switchState(&gsm->wander_state);
}

void GenericShootState::exit()
{
}
