#pragma once

#include <utility>

using vec2 = std::pair<int, int>;
using vec2f = std::pair<float, float>;

#include <vector>

using ActionPoints = int;
using Path = std::vector<vec2>;
struct Entity
{
    unsigned texture = 0; // Image used as texture
    Path path; // Path to follow during turn
    double speed = 0.25; // The time between each tile moved ( Doesn't Affect Balancing )
    int health;

    // Turn Based Action Points
    ActionPoints currentAP = 0;
    ActionPoints baseAP = 0;
};
