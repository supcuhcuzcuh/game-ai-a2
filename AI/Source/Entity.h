#pragma once
#include "Maze.h"
#include "ObjectBase.h"

struct Entity : public ObjectBase
{
    std::string type;
    MazePt position;
    // MazePt scale; // Not actually a Point in the Maze, MazePt just works as a Vector2
    int texture;
    int drawLayer = 2;

    virtual void update(double deltaTime) = 0;
};
