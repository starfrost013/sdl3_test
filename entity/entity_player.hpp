/* Player code */

#pragma once

#include <Capy.hpp>
#include <entity/entity.hpp>
#include <entity/entity_physical.hpp>
// include asset types here at the end so other parts of the engine only hav 

namespace Capy
{
    class PlayerEntity : public PhysicalEntity 
    {
        public:
            static constexpr const char* classname = "Player"; 

            virtual void Create() override;     // Called on object creation.
            virtual void Tick() override;       // Called every tick.
            virtual void Render() override;     // Called every frame.
            virtual void Destroy() override;    // Called on object destructioe
    };
}

