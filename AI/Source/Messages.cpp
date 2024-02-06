#include "Messages.h"

PathfindMessage::PathfindMessage(MazePt s, MazePt e)
{
    start = s;
    end = e;
}

PathfindMessage::PathfindMessage() { }

GetRandomEmptyPositionSubSectionMessage::GetRandomEmptyPositionSubSectionMessage(MazePt bl, MazePt tr)
{
    bottomLeft = bl;
    topRight = tr;
}

FindNearestEntity::FindNearestEntity(Entity* whoasked, const std::string& t)
{
    who_asked = whoasked;
    type = t;
}

SpawnEntityMessage::SpawnEntityMessage(Entity* toSpawn, unsigned texture)
{
    toIntroduceIntoTheScene = toSpawn;
    toIntroduceIntoTheScene->texture = texture;
}

KillEntityMessage::KillEntityMessage(Entity* toKill)
{
    deathRow = toKill;
}
