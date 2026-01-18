/* 
    Capybara Wars

    Quadtree.hpp: Quad Tree implementation

    A easy to use and efficiently updatable quadtree.
*/
#pragma once

#include <Capy.hpp>

namespace Capy
{
    class Quad
    {

    public:
        Vector2<float> position;
        Vector2<int32_t> size;
    
        /* Fixed size is faster */
        Quad* children[4];

        Quad()
        {
            position = Vector2<float>(), size = Vector2<int32_t>();
        }

        Quad(Vector2<int32_t> size)
        {
            position = Vector2<float>(), this->size = size;
        }

        Quad(Vector2<float> position, Vector2<int32_t> size)
        {
            position = Vector2<float>(), this->size = size;
        }

        void Divide()
        {
            int32_t halfWidth = size.x >> 1;
            int32_t halfHeight = size.y >> 1;

            children[0] = new Quad(Vector2<float>(position.x, position.y), 
            Vector2<int32_t>(halfWidth, halfHeight));
            children[1] = new Quad(Vector2<float>(position.x + halfWidth, position.y), 
            Vector2<int32_t>(halfWidth, halfHeight));
            children[2] = new Quad(Vector2<float>(position.x, position.y + halfHeight),
            Vector2<int32_t>(halfWidth, halfHeight));
            children[3] = new Quad(Vector2<float>(position.x + halfWidth, position.y + halfHeight), 
            Vector2<int32_t>(halfWidth, halfHeight));
        }

        bool IsDivisible() const
        {
            return (size.x > 1 && size.y > 1);
        }
        

    };

    class QuadTree
    {
        Quad root; 
    };
};

