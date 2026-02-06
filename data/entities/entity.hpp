#pragma once
#include <Capy.hpp>

/* entity.hpp: Core Entity code */

namespace Capy
{
    class Entity 
    { 
        public: 
            const char* classname; 

            
            // 
            // Events
            //
            virtual void Create() = 0;              // Called on object creation.
            virtual void Tick() = 0;                // Called every tick.
            virtual void Render() = 0;              // Called every frame.
            virtual void Destroy() = 0;             // Called on object destruction

    };
}

