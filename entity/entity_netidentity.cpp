// "Network Identities" are used to know what to spawn when a RPC to create an entity is receied

#include <core/game.hpp>
#include <entity/entity.hpp>
#include <entity/entity_player.hpp>

namespace Capy
{
    // this is all a test


    Entity* NetIdentity_SpawnPlayer() { return (Entity*)new PlayerEntity; };
    
    std::unordered_map<const char*, Entity* (*)()> Entity::netIdentities = 
    {
        { PlayerEntity::classname, NetIdentity_SpawnPlayer },
        
    };
}
