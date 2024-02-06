#pragma once

// Vector 2 Templated Struct (Templates allow for floats instead of ints)

struct vec2f;

struct vec2
{
    int x;
    int y;

    vec2 operator+(const vec2& other);
    vec2 operator-(const vec2& other);
    void operator+=(const vec2& other);
    void operator-=(const vec2& other);
    bool operator==(const vec2& other);
    bool operator!=(const vec2& other);
    bool operator<(const vec2& other) const;
    bool operator>(const vec2& other) const;
    float length();

    vec2f fromInt();
};

struct vec2f
{
    float x;
    float y;

    vec2f operator+(const vec2f& other);
    vec2f operator-(const vec2f& other);
    void operator+=(const vec2f& other);
    void operator-=(const vec2f& other);
    bool operator==(const vec2f& other);
    bool operator!=(const vec2f& other);
    bool operator<(const vec2f& other) const;
    bool operator>(const vec2f& other) const;
    float length();

    vec2 fromFloat();
};
