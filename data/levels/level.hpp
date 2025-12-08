/* Level.hpp: Level definitions */

#include <Capy.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    #define LEVEL_FORMAT_VERSION            1

    /* Defines the level header */
    struct LevelHeader 
    {
        uint32_t format_version;            // Level format version
        uint32_t size_x;
        uint32_t size_y; 
    };

    /* The struct of the current level */
    class Level 
    {
        LevelHeader header;

        WorldEntity world; 

        /* ALL OTHER ENTITIES GO HERE */
        std::vector<Entity> entities;

        void Load(const char* path);

    };

}
