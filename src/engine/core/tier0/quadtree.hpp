/* 
    Capybara Wars

    Quadtree.hpp: Quad Tree implementation

    A easy to use and efficiently updatable quadtree. Used for world representation and collision.
*/
#pragma once

#include <core/tier0/vector2.hpp>

#define QUAD_NUM_CHILDREN           4

namespace Capy
{
    template <typename T>
    class Quad
    {

    public:
        Vector2<float> position;
        Vector2<int32_t> size;
        T thing;

        enum QuadDivideType
        {
            DIVIDE_2BY2 = 0,        // this node has 4 children that make up a 2x2 square
            DIVIDE_1BY2 = 1,        // this node has 2 children that make up a 1x2 square
            DIVIDE_2BY1 = 2,        // this node has 2 children that make up a 2x1 square
            DIVIDE_NONE = 3,        // this node has no children
        };

        QuadDivideType divideType;

        /* Fixed size is faster */
        Quad* children[4];

        Quad()
        {
            position = Vector2<float>(), size = Vector2<int32_t>();
            children[0] = children[1] = children[2] = children[3] = nullptr;
            divideType = Quad::QuadDivideType::DIVIDE_NONE;
        }

        Quad(Vector2<int32_t> size)
        {
            position = Vector2<float>(), this->size = size;
            children[0] = children[1] = children[2] = children[3] = nullptr;
            divideType = Quad::QuadDivideType::DIVIDE_NONE;

        }

        Quad(Vector2<float> position, Vector2<int32_t> size)
        {
            position = Vector2<float>(), this->size = size;
            children[0] = children[1] = children[2] = children[3] = nullptr;
            divideType = Quad::QuadDivideType::DIVIDE_NONE;
        }

        ~Quad()
        {
            /* 
                Clean out the tree
                This will recursively call the destructors
            */
            for (size_t i = 0; i < QUAD_NUM_CHILDREN; i++)
            {
                if (children[i] != nullptr)
                    delete children[i];
            }
        }

        void Divide(QuadDivideType type = Quad::QuadDivideType::DIVIDE_2BY2)
        {
            int32_t halfWidth = size.x >> 1;
            int32_t halfHeight = size.y >> 1;

            divideType = type;

            switch (type)
            {
                case Quad::QuadDivideType::DIVIDE_1BY2:
                    children[0] = new Quad(Vector2<float>(position.x, position.y), 
                    Vector2<int32_t>(size.x, halfHeight));
                    children[1] = new Quad(Vector2<float>(position.x, position.y + halfHeight), 
                    Vector2<int32_t>(size.x, halfHeight));
                    children[2] = children[3] = nullptr; // to be safe
                    break; 
                case Quad::QuadDivideType::DIVIDE_2BY1:
                    children[0] = new Quad(Vector2<float>(position.x, position.y), 
                    Vector2<int32_t>(halfWidth, size.y));
                    children[1] = new Quad(Vector2<float>(position.x + halfWidth, position.y), 
                    Vector2<int32_t>(halfWidth, size.y));
                    children[2] = children[3] = nullptr; // to be safe
                    break; 
                case Quad::QuadDivideType::DIVIDE_2BY2: // 2x2 divide
                    children[0] = new Quad(Vector2<float>(position.x, position.y), 
                    Vector2<int32_t>(halfWidth, halfHeight));
                    children[1] = new Quad(Vector2<float>(position.x + halfWidth, position.y), 
                    Vector2<int32_t>(halfWidth, halfHeight));
                    children[2] = new Quad(Vector2<float>(position.x, position.y + halfHeight),
                    Vector2<int32_t>(halfWidth, halfHeight));
                    children[3] = new Quad(Vector2<float>(position.x + halfWidth, position.y + halfHeight), 
                    Vector2<int32_t>(halfWidth, halfHeight));
                    break; 
            }

            /* at least two children are always guaranteed so we don't need to do a nullptr check for this */
        }

        bool IsDivisible() const
        {
            return (size.x > 1 && size.y > 1);
        }
        
        bool HasChildren() const
        {
            // We ensure that all 4 are set to NULL and that nodes either have none, two or four children so we can only check if child 0 is null
            return (children[0] != nullptr);
        }

    };

    template <typename T>
    class QuadTree
    {
    public:
        Quad<T>* root; 

        /* probably a good idea to prevent us from destroying the root */
        Quad<T> GetRoot() // get the Yggdrasil 
        {
            return root; 
        } 

        QuadTree()
        {
            root = new Quad<T>();
        }

        ~QuadTree() 
        {
            delete root; 
        }

    };
};

