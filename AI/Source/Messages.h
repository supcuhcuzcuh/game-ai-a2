#pragma once
#include "Entity.h"
#include "Maze.h"
#include "Message.h"

struct PathfindMessage : public Message
{
    MazePt end;
    MazePt start;
    std::vector<MazePt> route;
    bool gotRoute = false;

    PathfindMessage(MazePt s, MazePt e);
    PathfindMessage();
};

struct GetRandomEmptyPositionMessage : public Message
{
    // Return Values
    int x;
    int y;
};

struct GetRandomEmptyPositionSubSectionMessage : public Message
{
    // Return Values
    int x;
    int y;

    // Requirements
    MazePt bottomLeft;
    MazePt topRight;

    GetRandomEmptyPositionSubSectionMessage(MazePt bl, MazePt tr);
};

struct FindNearestEntity : public Message
{
    std::string type; // Give me a Type to Find
    Entity* who_asked = nullptr;

    // Here is their position
    Entity* nearest = nullptr;

    FindNearestEntity(Entity* whoasked, const std::string& t);
};

struct SpawnEntityMessage : public Message
{
    Entity* toIntroduceIntoTheScene;
    SpawnEntityMessage(Entity* toSpawn, unsigned texture);
};

struct KillEntityMessage : public Message
{
    Entity* deathRow;
    KillEntityMessage(Entity* toKill);
};
