// entity_world.cpp: World generation code

#include <core/core.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_world.hpp>
#include <util/util.hpp>

namespace Capy
{ 
    //
    // Typedefs to prevent the code looking like the average C++ error message.
    //
    typedef std::unordered_map<WorldTile*, uint8_t> TileToIndexMap;
    typedef std::unordered_map<uint8_t, WorldTile*> IndexToTileMap;

    //
    // TILE DEFINITIONS
    //
    WorldTile WorldTileAir = { .colourR = 0, .colourG = 0, .colourB = 0, .colourA = 0, 
        .texture_path = nullptr, .name = "Air [No render]", .emitsLight = false, };
    WorldTile WorldTileGrass = { .colourR = 20, .colourG = 170, .colourB = 50, .colourA = 255, 
        .texture_path = nullptr, .name = "Grass",  .emitsLight = false, };
    WorldTile WorldTileStone = { .colourR = 180, .colourG = 175, .colourB = 185, .colourA = 255, 
        .texture_path = nullptr, .name = "Stone",  .emitsLight = false, };
    WorldTile WorldTileLava = { .colourR = 255, .colourG = 134, .colourB = 35, .colourA = 255, 
        .texture_path = nullptr, .name = "Lava",  .emitsLight = true, };
    WorldTile WorldTileSand = { .colourR = 180, .colourG = 175, .colourB = 185, .colourA = 255, 
        .texture_path = nullptr, .name = "Sand", .emitsLight = false, };
    WorldTile WorldTileGlass = { .colourR = 241, .colourG = 243, .colourB = 242, .colourA = 127, 
        .texture_path = nullptr, .name = "Glass", .emitsLight = false, };
    WorldTile WorldTileCrystal = { .colourR = 240, .colourG = 186, .colourB = 255, .colourA = 255, 
        .texture_path = nullptr, .name = "Crystal Meth", .emitsLight = false, };
    WorldTile WorldTileWater = { .colourR = 4, .colourG = 24, .colourB = 155, .colourA = 127, 
        .texture_path = nullptr, .name = "Water", .emitsLight = false, };

    /* 
        for each of maintenance these are separate lists.
        search is faster than using an array
        
        it would make level generation slightly faster to have this the "wrong way around" so the level generation code cna use the pointers to find the indices,
        but rendering would be slower - and level generation is only done once.

        having two lists was rejected due to data duplication issues
    */
    IndexToTileMap tileIndices = 
    {
        { 0, &WorldTileAir, },
        { 1, &WorldTileGrass, },
        { 2, &WorldTileStone, },
        { 3, &WorldTileLava, },
        { 4, &WorldTileSand, },
        { 5, &WorldTileGlass, },
        { 6, &WorldTileCrystal, },
        { 7, &WorldTileWater, },
    };


    // THESE ARE IN COORDINATES
    std::unordered_map<uint32_t, WorldTile*> heightData = 
    {
        { 0, &WorldTileAir },
        { 16, &WorldTileGrass },
        { 28, &WorldTileStone },
        { 64, &WorldTileLava },
    };

    void WorldEntity::CreateGenerateNoise()
    {
        Logging_LogChannel("[Phase 1] Generating noisemap (%d points, %d variance)", LogChannel::Debug,
        NOISE_STEPS, NOISE_MAX_VARIANCE);

        for (uint32_t i = 0; i < NOISE_STEPS; i++)
        {
            noiseData[i] = (float)(Util_RandomSingle() * NOISE_MAX_VARIANCE) / TILE_SIZE_Y; 
        }
    }

    void WorldEntity::CreateGenerateWorld()
    {

        if (header.size.x == 0
        || header.size.y == 0)
        {
            Logging_LogChannel("Tried to create a world with a size of 0 in at least one dimension.", LogChannel::Fatal);
            return;
        }

        /* 
            optimisation:
            
            it would be slow to search through the whole list of indicies every time but we don't want to dupliate the data - it makes it easier to create bugs
            instead we can build a temporary std::unordered_map of pointers to indices

            this basically flips the map
        */

        TileToIndexMap pointersToIndicesMap;

        for (IndexToTileMap::const_iterator it = tileIndices.begin(); it != tileIndices.end(); ++it)
            pointersToIndicesMap.insert({ it->second, it->first });

        uint32_t mapSizeBytes = header.size.x * header.size.y;

        Logging_LogChannel("[Phase 2] Allocating memory for map (size will be %d bytes, %d x %d tiles, %d x %d pixels)", 
            LogChannel::Debug, 
        mapSizeBytes, header.size.x, header.size.y, (header.size.x * TILE_SIZE_X), (header.size.y * TILE_SIZE_Y));

        world = new uint8_t[mapSizeBytes];

        // 32bpp so 1 index = 4 bytes
        uint32_t firstGroundY = 0, data = 0;        
    
        WorldTile* currentTile = heightData[0];

        Logging_LogChannel("[Phase 3] Generating terrain layers...", LogChannel::Debug);

#ifdef DEBUG
        uint32_t layerNumber = 0;  
#endif

        // size is simply screen size for now
        // set up initial operation
        for (uint32_t y = 0; y < header.size.y; y += TILE_SIZE_Y)
        {
            // this seems like an expensive operation but it's only done at creation time.
            // invalid elements construct a nullptr in STL
            // maybe generate an array instead 
            WorldTile* newTileCandidate = heightData[y];

            if (y > 0 
            && newTileCandidate != currentTile
            && newTileCandidate != nullptr)
            {
                currentTile = newTileCandidate;
#ifdef DEBUG
                layerNumber++;  

                Logging_LogChannel("[Phase 3.%d] Generating layer for tile: %s", LogChannel::Debug, layerNumber, currentTile->name);
#endif

                if (currentTile != &WorldTileAir
                && firstGroundY == 0)
                    firstGroundY = y;
            }
        }

        uint32_t step = header.size.x/NOISE_STEPS;
        uint32_t stepCurrent = 0;       // current step
        uint32_t stepProgress = 0;      // current progresss within the step

        //reset currenttile so it can be selected again...we don't need to create the hashtable entries again so it should bef aster
        currentTile = heightData[0];

        for (uint32_t y = 0; y < header.size.y; y++)
        {
            // this seems like an expensive operation but it's only done at creation time.
            // invalid elements construct a nullptr in STL
            // maybe generate an array instead 
            WorldTile* newTileCandidate = heightData[y];

            // this part needs to be duplicated
            if (y > 0 
            && newTileCandidate != currentTile
            && newTileCandidate != nullptr)
            {
                //reroll noise
                currentTile = newTileCandidate;
                CreateGenerateNoise();
            }

            for (uint32_t x = 0; x < header.size.x; x++)
            {
                //
                // NOISE
                // 

                uint32_t adjustedY = y;

                // make sure the noise step
                stepProgress += TILE_SIZE_X;

                if (stepProgress > step)
                {
                    stepProgress = 0;
                    stepCurrent++;
                }

                if (y >= firstGroundY) // temporary hack
                {
                    // sanity check
                    if (stepCurrent < NOISE_STEPS)
                    {
                        float noiseCurrent = noiseData[stepCurrent];
                        float noiseNext = noiseCurrent;
                        if (stepCurrent < (NOISE_STEPS - 1)) // prevent overflow
                            noiseNext = noiseData[stepCurrent + 1];
                        
                        //lerp
                        float noise = (noiseCurrent + ((noiseNext - noiseCurrent) * ((float)stepProgress / (float)step)));
                        adjustedY -= noise;  // subtract
                    }
                }

                // Aggressive anti-memory corruption activities
                if (adjustedY > (header.size.y - TILE_SIZE_Y))
                    adjustedY = header.size.y - TILE_SIZE_Y;
                        
                if (adjustedY < 0)
                    adjustedY = 0;

                uint32_t index = (adjustedY * header.size.x) + x;

                world[index] = pointersToIndicesMap[currentTile];
            }

            // reset step counter
            stepCurrent = stepProgress = 0;
        }  

        Logging_LogChannel("[Phase 4] Terrain generation done. Generating quadmap for collision... (wip)", LogChannel::Debug, layerNumber, currentTile->name);

        // Now the generation of the world data is done.
        // So we can generate the quadtree.

        // generate root quadtile children
        collision.root->Divide();

        uint32_t currentQuadScaleX = header.size.x;
        uint32_t currentQuadScaleY = header.size.y;

        Quad<uint8_t*>* current = collision.root;

        while (currentQuadScaleX >= TILE_SIZE_X
        || currentQuadScaleY >= TILE_SIZE_Y)
        {
            /* identical objects are merged into the same quad */
            for (int32_t y = 0; y < header.size.y; y += currentQuadScaleY)
            {

            }

            // gen step done
            if (currentQuadScaleX > TILE_SIZE_X)
                currentQuadScaleX >>= 1;

            if (currentQuadScaleY > TILE_SIZE_Y)
                currentQuadScaleY >>= 1;
        }

        /*
       
    */
    }

    void WorldEntity::Create()
    {
        Logging_LogChannel("World Generation:", LogChannel::Debug);


        CreateGenerateNoise();
        CreateGenerateWorld();

        // go
    }

    // Called during level loading
    void WorldEntity::Deserialise()
    {

    }

    void WorldEntity::Render()
    {
        uint32_t* texturePixels;  
        uint32_t index; 
        int32_t pitch; 

        SDL_Rect newRect = { 0, 0, (int32_t)game.settings.screenX, (int32_t)game.settings.screenY };

        // mock 'sky' colour 
        SDL_SetRenderDrawColor(game.renderer, 30, 50, 180, 255);

        /* Todo: DETECT world change */

        SDL_LockTexture(game.renderTarget, &newRect, (void**)&texturePixels, &pitch);

         // 32bpp so 1 index = 4 bytes
        uint32_t firstGroundY = 0;
        uint32_t lastLightBorderY = 0; 

        WorldTile* currentTile = heightData[0];
        
        for (uint32_t y = 0; y < game.settings.screenY; y += TILE_SIZE_Y)
        {
            // blit to texture

            for (uint32_t x = 0; x < game.settings.screenX; x += TILE_SIZE_X)
            {
                uint32_t xTile = x / TILE_SIZE_X;
                uint32_t yTile = y / TILE_SIZE_Y;

                if (xTile >= header.size.x
                || yTile >= header.size.y)
                {
                    goto done;
                }

                uint32_t worldIndex = (yTile * header.size.x) + xTile;

                uint8_t index = world[worldIndex];

                // don't draw out of bounds as random 'tiles' just in case - might not be the fastest code
                if (index > (tileIndices.size()))
                    return;

                WorldTile* currentTile = tileIndices[index];

                // vary colours per tile so it looks cooler
                // todo: make this not run on every frame
                int32_t finalColourR = currentTile->colourR + rand() % 12;
                int32_t finalColourG = currentTile->colourG + rand() % 12;
                int32_t finalColourB = currentTile->colourB + rand() % 12;

                int32_t subtractFactor = firstGroundY;

                if (currentTile->emitsLight)
                    subtractFactor = lastLightBorderY;
                    
                // always faster to bitshift
                finalColourR -= (subtractFactor) >> 1;
                finalColourG -= (subtractFactor) >> 1;
                finalColourB -= (subtractFactor) >> 1;

                // clamp colours
                if (finalColourR > 255) finalColourR = 255; 
                if (finalColourG > 255) finalColourG = 255;
                if (finalColourB > 255) finalColourB = 255;

                // Noise introduction made pixels sometimes show up above the colour. So let's just ignore it if it's below 0
                if (finalColourR < 0) finalColourR = currentTile->colourR; 
                if (finalColourG < 0) finalColourG = currentTile->colourG;
                if (finalColourB < 0) finalColourB = currentTile->colourB;

                // draw tiles
                for (uint32_t yy = 0; yy < TILE_SIZE_Y; yy++)
                {
                    for (uint32_t xx = 0; xx < TILE_SIZE_X; xx++)
                    {
                        uint32_t value = ((currentTile->colourA & 0xFF) << 24)
                            | ((finalColourR & 0xFF) << 16) 
                            | ((finalColourG & 0xFF) << 8)
                            | ((finalColourB));

                        uint32_t textureLocation = (((y + yy) * pitch) + ((x + xx) << 2)) >> 2;

                        texturePixels[textureLocation] = value;
                    }
                }
            }
        }

done:
        SDL_UnlockTexture(game.renderTarget);
        SDL_RenderTexture(game.renderer, game.renderTarget, NULL, NULL);
    }

    void WorldEntity::Tick()
    {

    }

    void WorldEntity::Destroy()
    {

    }
}
