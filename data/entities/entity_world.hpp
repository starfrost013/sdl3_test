#include <data/entities/entity.hpp>

namespace Capy
{
    /* World tile representation */
    struct LevelTile 
    {
        uint32_t colourR;
        uint32_t colourG;
        uint32_t colourB;
        uint32_t colourA;
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
        
        // 1D perlin noise steps. We interpolate between these.
        #define NOISE_STEPS         36
        #define NOISE_MAX_VARIANCE  72       

        void Create();
        void Deserialise();
        void Render(); 
        void Tick();
        void Destroy();


    private: 
        void CreateGenerateWorld(uint32_t* texture_pixels, int32_t pitch);
        void CreateGenerateNoise();

        Vector2 size; 

        /* Tilemap */


        float noiseData[NOISE_STEPS];


    };
}

