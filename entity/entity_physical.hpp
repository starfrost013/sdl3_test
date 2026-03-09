/* Player code */

#pragma once

#include <Capy.hpp>
#include <entity/entity.hpp>
#include <core/render/assets/asset_texture.hpp>

namespace Capy
{
    class PhysicalEntity : public Entity 
    {
        public:
            static constexpr const char* classname = "Generic Physical Entity"; 

            virtual void Create();     // Called on object creation.
            virtual void Tick();       // Called every tick.
            virtual void Render();     // Called every frame.
            virtual void Destroy();    // Called on object destruction

        private:
            AssetTexture* texture; 
    };
}

