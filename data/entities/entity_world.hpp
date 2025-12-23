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
        
        //
        // DEFINES
        //
        #define WIDTH_IN_BLOCKS(x)  (x*TILE_SIZE_X)
        #define HEIGHT_IN_BLOCKS(x) (x*TILE_SIZE_Y)
        #define TILE_SIZE_X         8
        #define TILE_SIZE_Y         8

        #define NOISE_STEPS         32

        void Create();
        void Render(); 
        void Tick();
        void Destroy();


    private: 
        void CreateGenerateWorld(uint32_t* texture_pixels, int32_t pitch);
        void CreateGenerateNoise();

        float NoiseData[NOISE_STEPS];


    };
}

