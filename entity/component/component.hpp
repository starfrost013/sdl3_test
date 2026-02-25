#pragma once
#include <Capy.hpp>

// 
// component.hpp
// ECS base
//

namespace Capy
{
    class Component
    {
        public: 
            const char* name;

            virtual void Init() { };
            virtual void Tick() { };
            virtual void Frame() { }; 
        private:

    }
}