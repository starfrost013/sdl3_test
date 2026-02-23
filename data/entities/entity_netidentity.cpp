#include <core/game.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_player.hpp>

namespace Capy
{
    std::unordered_map<const char*, Entity (*func)()> netIdentities = 
    {
        { "PlayerEntity", new PlayerEntity, }
    };
}
