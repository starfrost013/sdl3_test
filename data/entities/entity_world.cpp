#include <core/core.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{ 
    // P2.0 only
    LevelTile tileData[] = 
    {
        { .colour_r = 0, .colour_g = 0, .colour_b = 0, .colour_a = 0, .texture_path = nullptr, },               // air
        { .colour_r = 20, .colour_g = 170, .colour_b = 50, .colour_a = 255, .texture_path = nullptr, },         // grass
        { .colour_r = 180, .colour_g = 175, .colour_b = 185, .colour_a = 255, .texture_path = nullptr, },       // stone
        { .colour_r = 255, .colour_g = 134, .colour_b = 35, .colour_a = 255, .texture_path = nullptr, },        // lava
        { .colour_r = 180, .colour_g = 175, .colour_b = 185, .colour_a = 255, .texture_path = nullptr, },       // sand
        { .colour_r = 241, .colour_g = 243, .colour_b = 242, .colour_a = 127, .texture_path = nullptr, },       // glass
        { .colour_r = 240, .colour_g = 186, .colour_b = 255, .colour_a = 255, .texture_path = nullptr, },       // crystal (gets you high, has practical gameplay effects)    }; 
    };

    #define NUM_TILES   sizeof(tileData)/sizeof(tileData[0])
    #define TILE_SIZE_X 16
    #define TILE_SIZE_Y 16

    void WorldEntity::Create()
    {
        uint32_t* texture_pixels;  
        uint32_t index; 
        int32_t pitch; 

        SDL_Rect new_rect;

        new_rect.x = new_rect.y = 0;
        new_rect.w = game.settings.screen_x;
        new_rect.h = game.settings.screen_y;

        SDL_LockTexture(game.render_target, &new_rect, (void**)&texture_pixels, &pitch);

        // 32bpp so 1 index = 4 bytes
        LevelTile currentTile;

        // size is simply screen size for now
        for (uint32_t y = 0; y < game.settings.screen_y; y += TILE_SIZE_Y)
        {
            for (uint32_t x = 0; x < game.settings.screen_x; x += TILE_SIZE_X)
            {
                currentTile = tileData[rand() % NUM_TILES];

                // draw 4x4
                for (uint32_t yy = 0; yy < TILE_SIZE_Y; yy++)
                {
                    for (uint32_t xx = 0; xx < TILE_SIZE_X; xx++)
                    {
                        // >> 2 because 32bpp
                        uint32_t index = (((y + yy) * pitch) + ((x + xx) << 2)) >> 2;

                        texture_pixels[index] = ((currentTile.colour_a & 0xFF) << 24)
                            | ((currentTile.colour_r & 0xFF) << 16) 
                            | ((currentTile.colour_g & 0xFF) << 8)
                            | ((currentTile.colour_b));
                    }
                }

            }
        }        

        // go
        SDL_UnlockTexture(game.render_target);
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
