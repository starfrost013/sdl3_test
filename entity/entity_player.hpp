/* Player code */

#include <Capy.hpp>
#include <Capy.hpp>
#include <entity/entity.hpp>

namespace Capy
{
    class PlayerEntity : public Entity 
    {
        public:
            virtual void Generate();              // Called on object creation.
            virtual void Tick();                // Called every tick.
            virtual void Render();              // Called every frame.
            virtual void Destroy();             // Called on object destruction

        private:
    };
}

