/* A basic 2D vector */
#pragma once
#include "SDL3/SDL_scancode.h"
#include <Capy.hpp>

namespace Capy
{
    template <typename T>
    class Vector2
    {
    public:
        T x, y;

        Vector2()
        {
            x = y = 0;
        }

        Vector2(T x, T y)
        {
            this->x = x;
            this->y = y; 
        }

        Vector2 operator+(const Vector2& other)
        {
            Vector2<T> v;
            v.x = x + other.x;
            v.y = y + other.y;
            return v;
        }

        Vector2 operator-(const Vector2& other)
        {
            Vector2<T> v;
            v.x = x - other.x;
            v.y = y - other.y;
            return v;
        }
        
        Vector2 operator*(const Vector2& other)
        {
            Vector2<T> v;
            v.x = x * other.x;
            v.y = y * other.y;
            return v;
        }

        Vector2 operator/(const Vector2& other)
        {
            Vector2<T> v;
            v.x = x / other.x;
            v.y = y / other.y;
            return v;
        }
    };
}