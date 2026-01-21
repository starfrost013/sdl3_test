#include <core/core.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_world.hpp>
#include <util/util.hpp>

namespace Capy
{ 
    WorldTile WorldTileAir = { .colourR = 0, .colourG = 0, .colourB = 0, .colourA = 0, .texture_path = nullptr, .emitsLight = false, };
    WorldTile WorldTileGrass = { .colourR = 20, .colourG = 170, .colourB = 50, .colourA = 255, .texture_path = nullptr, .emitsLight = false, };
    WorldTile WorldTileStone = { .colourR = 180, .colourG = 175, .colourB = 185, .colourA = 255, .texture_path = nullptr, .emitsLight = false, };
    WorldTile WorldTileLava = { .colourR = 255, .colourG = 134, .colourB = 35, .colourA = 255, .texture_path = nullptr, .emitsLight = true, };
    WorldTile WorldTileSand = { .colourR = 180, .colourG = 175, .colourB = 185, .colourA = 255, .texture_path = nullptr, .emitsLight = false, };
    WorldTile WorldTileGlass = { .colourR = 241, .colourG = 243, .colourB = 242, .colourA = 127, .texture_path = nullptr, .emitsLight = false, };
    WorldTile WorldTileCrystal = { .colourR = 240, .colourG = 186, .colourB = 255, .colourA = 255, .texture_path = nullptr, .emitsLight = false,  };
    WorldTile WorldTileWater = { .colourR = 4, .colourG = 24, .colourB = 155, .colourA = 127, .texture_path = nullptr, .emitsLight = false,  };

    // THESE ARE IN COORDINATES
    std::unordered_map<uint32_t, WorldTile*> heightData = 
    {
        { HEIGHT_IN_BLOCKS(0), &WorldTileAir },
        { HEIGHT_IN_BLOCKS(16), &WorldTileGrass },
        { HEIGHT_IN_BLOCKS(28), &WorldTileStone },
        { HEIGHT_IN_BLOCKS(64), &WorldTileLava },
    };

    void WorldEntity::CreateGenerateNoise()
    {
        for (uint32_t i = 0; i < NOISE_STEPS; i++)
        {
            noiseData[i] = (float)(Util_RandomSingle() * NOISE_MAX_VARIANCE); 
        }
    }

    void WorldEntity::CreateGenerateWorld(uint32_t* texture_pixels, int32_t pitch)
    {

        // generate root quadtile children
        collision.root->Divide();

        uint32_t currentQuadScaleX = header.size.x;
        uint32_t currentQuadScaleY = header.size.y;

        Quad<WorldTile>* current = collision.root;

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
        // 32bpp so 1 index = 4 bytes
        uint32_t firstGroundY = 0;
        uint32_t lastLightBorderY = 0; 
        
        WorldTile* currentTile = heightData[0];

        // size is simply screen size for now
        // set up initial operation
        for (uint32_t y = 0; y < game.settings.screenY; y += TILE_SIZE_Y)
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

                if (currentTile != &WorldTileAir
                && firstGroundY == 0)
                    firstGroundY = y;

                if (currentTile->emitsLight)
                    lastLightBorderY = y;
            }
        }

        uint32_t step = game.settings.screenX/NOISE_STEPS;
        uint32_t stepCurrent = 0;       // current step
        uint32_t stepProgress = 0;      // current progresss within the step

        //reset currenttile so it can be selected again...we don't need to create the hashtable entries again so it should bef aster
        currentTile = heightData[0];

        // size is simply screen size for now
        for (uint32_t y = 0; y < game.settings.screenY; y += TILE_SIZE_Y)
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

            for (uint32_t x = 0; x < game.settings.screenX; x += TILE_SIZE_X)
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
                if (adjustedY > (game.settings.screenY - TILE_SIZE_Y))
                    adjustedY = game.settings.screenY - TILE_SIZE_Y;
                        
                if (adjustedY < 0)
                    adjustedY = 0;

                // vary colours per tile so it looks cooler
                int32_t finalColourR = currentTile->colourR + rand() % 12;
                int32_t finalColourG = currentTile->colourG + rand() % 12;
                int32_t finalColourB = currentTile->colourB + rand() % 12;

                int32_t subtractFactor = firstGroundY;

                if (currentTile->emitsLight)
                    subtractFactor = lastLightBorderY;
                    
                // always faster to bitshift
                finalColourR -= ((adjustedY - subtractFactor) >> 1);
                finalColourG -= ((adjustedY - subtractFactor) >> 1);
                finalColourB -= ((adjustedY - subtractFactor) >> 1);

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
                        // >> 2 because 32bpp
                        uint32_t index = (((adjustedY + yy) * pitch) + ((x + xx) << 2)) >> 2;

                        uint32_t value = ((currentTile->colourA & 0xFF) << 24)
                            | ((finalColourR & 0xFF) << 16) 
                            | ((finalColourG & 0xFF) << 8)
                            | ((finalColourB));

                        texture_pixels[index] = value;
                    }
                }
            }

            // reset step counter
            stepCurrent = stepProgress = 0;
        }        
    */
    }

    void WorldEntity::Create()
    {
        
        // mock 'sky' colour 
        SDL_SetRenderDrawColor(game.renderer, 30, 50, 180, 255);

        uint32_t* texture_pixels;  
        uint32_t index; 
        int32_t pitch; 

        SDL_Rect new_rect = { 0, 0, (int32_t)game.settings.screenX, (int32_t)game.settings.screenY};

        SDL_LockTexture(game.render_target, &new_rect, (void**)&texture_pixels, &pitch);

        CreateGenerateNoise();
        CreateGenerateWorld(texture_pixels, pitch);

        // go
        SDL_UnlockTexture(game.render_target);
    }

    // Called during level loading
    void WorldEntity::Deserialise()
    {

    }

    void WorldEntity::Render()
    {
        // world generation test code


        SDL_RenderTexture(game.renderer, game.render_target, NULL, NULL);
    }

    void WorldEntity::Tick()
    {

    }

    void WorldEntity::Destroy()
    {

    }
}
