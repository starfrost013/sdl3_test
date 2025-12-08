#pragma once
#include <Capy.hpp>

/* entity.hpp: Core Entity code */

class Entity 
{ 
    public: 
        const char* classname; 

        // 
        // Events
        //
        virtual void Create();              // Called on object creation.
        virtual void Tick();                // Called every tick.
        virtual void Render();              // Called every frame.
        virtual void Destroy();             // Called on object destruction

};