#include <data/entities/entity.hpp>

namespace Capy
{
    /* World tile representation */
    struct LevelTile 
    {
        uint32_t colour_r;
        uint32_t colour_g;
        uint32_t colour_b;
        uint32_t colour_a;
        const char* texture_path; 
        bool emitsLight;
    };


    /* 
        The class that represents the world.

        Levels are stored in the following order:

        Header
        WorldEntity
        All other entities
    */
    class WorldEntity : public Entity 
    {
    public: 
        
        void Create();
        void Render(); 
        void Tick();
        void Destroy();

    private: 

    };
}

