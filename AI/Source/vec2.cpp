#include "vec2.h"

#include <cmath>

vec2 vec2::operator+(const vec2& other)
{ return {x + other.x, y + other.y}; }
vec2 vec2::operator-(const vec2& other)
{ return {x - other.x, y - other.y}; }
void vec2::operator+=(const vec2& other)
{
    x += other.x;
    y += other.y;
}
void vec2::operator-=(const vec2& other)
{
    x -= other.x;
    y -= other.y;
}
bool vec2::operator==(const vec2& other)
{ return x == other.x && y == other.y; }
bool vec2::operator!=(const vec2& other)
{ return x != other.x || y != other.y; }
bool vec2::operator<(const vec2& other) const
{ return x < other.x && y < other.y; }
bool vec2::operator>(const vec2& other) const
{ return x > other.x && y > other.y; }
float vec2::length()
{ return std::sqrt((x*x) + (y*y)); }
vec2f vec2::fromInt()
{ return {static_cast<float>(x), static_cast<float>(y)}; }

vec2f vec2f::operator+(const vec2f& other)
{ return {x + other.x, y + other.y}; }
vec2f vec2f::operator-(const vec2f& other)
{ return {x - other.x, y - other.y}; }
void vec2f::operator+=(const vec2f& other)
{
    x += other.x;
    y += other.y;
}
void vec2f::operator-=(const vec2f& other)
{
    x -= other.x;
    y -= other.y;
}
bool vec2f::operator==(const vec2f& other)
{ return x == other.x && y == other.y; }
bool vec2f::operator!=(const vec2f& other)
{ return x != other.x || y != other.y; }
bool vec2f::operator<(const vec2f& other) const
{ return x < other.x && y < other.y; }
bool vec2f::operator>(const vec2f& other) const
{ return x > other.x && y > other.y; }
float vec2f::length()
{ return std::sqrt((x*x) + (y*y)); }
vec2 vec2f::fromFloat()
{ return {static_cast<int>(x), static_cast<int>(y)}; }
