/* Player code */

#include <Capy.hpp>
#include <Capy.hpp>
#include <data/entities/entity.hpp>

namespace Capy
{
    class PlayerEntity : public Entity 
    {
        public:
            virtual void Create();              // Called on object creation.
            virtual void Tick();                // Called every tick.
            virtual void Render();              // Called every frame.
            virtual void Destroy();             // Called on object destruction

        private:
            float x;
            float y; 

            float spawn_x;
            float spawn_y; 

            float dir_x;
            float dir_y; 

            float plane_x;
            float plane_y; 

            float move_speed;               // squares per second
            float rotation_speed;           // radians per second
    };
}

