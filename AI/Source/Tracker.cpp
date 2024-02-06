#include "Tracker.h"

Tracker::Tracker(MazePt* pos, float rate) : _position(pos), _rate(rate)
{
    _timer = 0;
    _index = 0;
}

void Tracker::setSpeed(float rate)
{ _rate = rate; }
float Tracker::getSpeed()
{ return _rate; }

void Tracker::reset()
{
    _timer = 0;
    _index = 0;
}

void Tracker::loadRoute(const std::vector<MazePt>& route)
{ _route = route; }

bool Tracker::update(double deltaTime)
{
    if (_route.empty()) return false;
    _timer += deltaTime;
    if (_timer > _rate)
    {
        *_position = _route[_index];
        ++_index;
        _timer = 0;
        if (_index >= _route.size())
            return false;
    }
    return true;
}
