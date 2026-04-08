// Implements a basic 2D vector class with basic operations.
#pragma once

#include <iostream>

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

    // Write out a Vector2 to a stream.
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
    {
        os << v.x;
        os << v.y;
        
        return os; 
    }

    // Read in a Vector2 from a stream. 
    template <typename T>
    std::istream& operator>>(std::istream& is, Vector2<T>& v)
    {
        is >> v.x;
        is >> v.y;
        
        return is; 
    }
}