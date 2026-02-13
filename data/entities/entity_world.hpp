#include "core/tier0/quadtree.hpp"
#include <data/entities/entity.hpp>

namespace Capy
{
    #define WORLD_NAME_LENGTH           48

    /* World tile representation */
    struct WorldTile 
    {
        uint8_t colourR;
        uint8_t colourG;
        uint8_t colourB;
        uint8_t colourA;
        const char* texture_path; 
        const char* name;
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

        friend class WorldHeader;
        
        //
        // DEFINES
        //
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

        /* 
            World file format header 
            A lot of this should only be accessed by the worldentity
        
        */
        struct WorldHeader
        {   
            uint32_t version; 
            Vector2<int32_t> size;
            
            char name[WORLD_NAME_LENGTH];
        public: 
            void SetSize(Vector2<int32_t> size) { this->size = size; };
        };

        WorldHeader& GetHeader() { return header; };

    private: 
    
        WorldHeader header;

        /* 
            World representation

            * A 2d array of 8-bit tile indicies (max 256 tiles, tile 0 is air)
            * A quadtree generated from that 2d array for collision detection purposes.
        */
        uint8_t* world;
        QuadTree<uint8_t*> collision;

        float noiseData[NOISE_STEPS];

        void CreateGenerateWorld();
        void CreateGenerateNoise();

        

    };
}

