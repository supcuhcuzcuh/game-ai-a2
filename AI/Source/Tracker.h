#pragma once
#include "Maze.h"

class Tracker
{
    MazePt* _position;
    float _rate;
    std::vector<MazePt> _route;
    double _timer;
    int _index;
    
public:
    Tracker() = default;
    Tracker(MazePt* pos, float rate);

    void setSpeed(float rate);
    float getSpeed();

    void reset();
    void loadRoute(const std::vector<MazePt>& route);

    bool update(double deltaTime);
};
