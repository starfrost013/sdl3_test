#pragma once
#include <Capy.hpp>
#include <entity/entity.hpp>
#include <world/world_generator.hpp>

namespace Capy
{

    // Type of the world
    enum WorldSeeder
    {
        WORLD_SEED_NONE = 0,
        WORLD_SEED_GENERATED = 1,
    };

    /* 
        Primary scene-type thing
    */

    class World
    {
        Entity worldSeed;
    };
}
