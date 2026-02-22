#pragma once
#include <Capy.hpp>
#include <data/entities/entity.hpp>

namespace Capy
{
    #define WORLD_NAME_LENGTH           48                          // name length
    #define WORLD_HEADER_MAGIC          0x59504143                  // header magic
    #define WORLD_FILE_FORMAT_VERSION   1                           // file format version number
    #define WORLD_DEFAULT_FILENAME      "worlds/World.capy"         // file default name

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
    class World 
    {
    public: 
        // let client access tileData
        friend class Client;
        friend class WorldHeader;
        
        //
        // DEFINES
        //
        #define TILE_SIZE_X         8
        #define TILE_SIZE_Y         8
        
        // 1D perlin noise steps. We interpolate between these.
        #define NOISE_STEPS         36
        #define NOISE_MAX_VARIANCE  72       

        //
        // CONSTRUCTORS (cannot provide explicit size in constructor for now)
        //

        ~World()
        {
            if (tileData)
                delete tileData;
        }

        //
        // METHODS
        //

        void Init(Vector2<int32_t> size);
        void Generate();
        bool Serialise(const char* fileName);
        bool Deserialise(const char* fileName);
        void Render(); 
        void Tick();
        void Destroy();

        /* 
            World file format header 
            A lot of this should only be accessed by the worldentity

            POD ONLY!
        
        */
        struct WorldHeader
        {   
            uint32_t magic = WORLD_HEADER_MAGIC; 
            uint16_t version = WORLD_FILE_FORMAT_VERSION; 
            Vector2<int32_t> size;
            
            char name[WORLD_NAME_LENGTH];
        };

        // Some stuff we're comfortable with getting but not setting...
        WorldHeader& GetHeader() { return header; };
        size_t GetSizeInBytes() { return (this->header.size.x * this->header.size.y); }; 
        uint8_t* GetWorldTileData() { return tileData; };

    private: 

        WorldHeader header;

        /* 
            World representation

            * A 2d array of 8-bit tile indicies (max 256 tiles, tile 0 is air)
            * A quadtree generated from that 2d array for collision detection purposes.
        */
        uint8_t* tileData;
        QuadTree<uint8_t*> collision;
        FilesystemFile* file; 

        float noiseData[NOISE_STEPS];

        bool OpenWorldFile(const char* fileName, bool tryCreate);      // open a world file
        void CloseWorldFile();

        void CreateGenerateWorld();
        void CreateGenerateNoise();
        void CreateGenerateQuadtree();
    };
}
