#include <core/game.hpp>
#include <entity/entity.hpp>
#include <entity/entity_player.hpp>

namespace Capy
{
    // this is all a test
    
    typedef enum NetIdentity
    {
        NETIDENTITY_PLAYER = 0x0,
        NETIDENTITY_PICKUP_GENERIC = 0x1,
    };

    Entity* NetIdentity_SpawnPlayer()
    {
        return (Entity*)new PlayerEntity;
    }

    std::unordered_map<int, Entity* (*)()> netIdentities = 
    {
        { NETIDENTITY_PLAYER, NetIdentity_SpawnPlayer },
    };
}
