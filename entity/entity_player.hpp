/* Player code */

#include <Capy.hpp>
#include <Capy.hpp>
#include <entity/entity.hpp>

namespace Capy
{
    class PlayerEntity : public Entity 
    {
        public:
            virtual void Create() override;     // Called on object creation.
            virtual void Tick() override;       // Called every tick.
            virtual void Render() override;     // Called every frame.
            virtual void Destroy() override;    // Called on object destruction

        private:
    };
}

