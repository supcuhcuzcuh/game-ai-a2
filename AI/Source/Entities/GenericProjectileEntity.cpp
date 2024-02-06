#include "GenericProjectileEntity.h"

#include "../Messages.h"
#include "../PostOffice.h"

GenericProjectileEntity::GenericProjectileEntity()
{
    // Register Projectile
    // PostOffice::GetInstance()->Register("Projectile",this);
    type = "PROJECTILE";
}

bool GenericProjectileEntity::Handle(Message* message)
{
    return false;
}

void GenericProjectileEntity::update(double deltaTime)
{
    _timer += deltaTime;
    _deathTimer += deltaTime;
    if (_timer > speed)
    {
        position.x += direction.x;
        position.y += direction.y;
        _timer = 0;
    }
    if (_deathTimer > lifeTime)
    {
        auto* killme = PostOffice::GetInstance()->QuickSend<KillEntityMessage>("MainScene",this);
        delete killme;
    }

    PathfindMessage* message = new PathfindMessage();
    message->start = position;
    message->end = {5,5}; // Target
    PostOffice::GetInstance()->Send("MainScene",message);

    auto* pathfindMessage = PostOffice::GetInstance()->QuickSend<PathfindMessage>("MainScene",position, MazePt{5,5});
}
