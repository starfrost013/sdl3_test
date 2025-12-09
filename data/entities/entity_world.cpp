#include <core/core.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{ 
    //
    // DEFINES
    //
    #define WIDTH_IN_BLOCKS(x)  (x*TILE_SIZE_X)
    #define HEIGHT_IN_BLOCKS(x) (x*TILE_SIZE_Y)
    #define TILE_SIZE_X         4
    #define TILE_SIZE_Y         4

    LevelTile levelTileAir = { .colour_r = 0, .colour_g = 0, .colour_b = 0, .colour_a = 0, .texture_path = nullptr, .emitsLight = false, };
    LevelTile levelTileGrass = { .colour_r = 20, .colour_g = 170, .colour_b = 50, .colour_a = 255, .texture_path = nullptr, .emitsLight = false, };
    LevelTile levelTileStone = { .colour_r = 180, .colour_g = 175, .colour_b = 185, .colour_a = 255, .texture_path = nullptr, .emitsLight = false, };
    LevelTile levelTileLava = { .colour_r = 255, .colour_g = 134, .colour_b = 35, .colour_a = 255, .texture_path = nullptr, .emitsLight = true, };
    LevelTile levelTileSand = { .colour_r = 180, .colour_g = 175, .colour_b = 185, .colour_a = 255, .texture_path = nullptr, .emitsLight = false, };
    LevelTile levelTileGlass = { .colour_r = 241, .colour_g = 243, .colour_b = 242, .colour_a = 127, .texture_path = nullptr, .emitsLight = false, };
    LevelTile levelTileCrystal = { .colour_r = 240, .colour_g = 186, .colour_b = 255, .colour_a = 255, .texture_path = nullptr, .emitsLight = false,  };
    LevelTile levelTileWater = { .colour_r = 4, .colour_g = 24, .colour_b = 155, .colour_a = 127, .texture_path = nullptr, .emitsLight = false,  };

    // THESE ARE IN COORDINATES
    std::unordered_map<uint32_t, LevelTile*> heightData = 
    {
        { HEIGHT_IN_BLOCKS(0), &levelTileAir },
        { HEIGHT_IN_BLOCKS(24), &levelTileGrass },
        { HEIGHT_IN_BLOCKS(72), &levelTileStone },
        { HEIGHT_IN_BLOCKS(180), &levelTileLava },
    };


    void WorldEntity::Create()
    {
        // mock 'sky' colour 
        SDL_SetRenderDrawColor(game.renderer, 30, 50, 180, 255);

        uint32_t* texture_pixels;  
        uint32_t index; 
        int32_t pitch; 

        SDL_Rect new_rect = { 0, 0, (int32_t)game.settings.screen_x, (int32_t)game.settings.screen_y};

        SDL_LockTexture(game.render_target, &new_rect, (void**)&texture_pixels, &pitch);

        // 32bpp so 1 index = 4 bytes
        uint32_t tile_y = 0, tile_x = 0;
        uint32_t last_light_border_y = 0; 

        LevelTile* currentTile = heightData[0];

        // size is simply screen size for now
        for (uint32_t y = 0; y < game.settings.screen_y; y += TILE_SIZE_Y)
        {
            for (uint32_t x = 0; x < game.settings.screen_x; x += TILE_SIZE_X)
            {
                // this seems like an expensive operation but it's only done at creation time.
                // invalid elements construct a nullptr in STL
                // maybe generate an array instead 
                LevelTile* newTileCandidate = heightData[y];

                if (y > 0 
                && newTileCandidate != currentTile
                && newTileCandidate != nullptr)
                {
                    currentTile = newTileCandidate;

                    if (currentTile->emitsLight)
                        last_light_border_y = y;
                }

                // vary colours per tile so it looks cooler
                int32_t final_colour_r = currentTile->colour_r + rand() % 12;
                int32_t final_colour_g = currentTile->colour_g + rand() % 12;
                int32_t final_colour_b = currentTile->colour_b + rand() % 12;

                // further down should be darker
                //final_colour_r -= ((y - last_border_y) / 2);
                //final_colour_g -= ((y - last_border_y) / 2);
                //final_colour_b -= ((y - last_border_y) / 2);

                if (!currentTile->emitsLight)
                {
                    final_colour_r -= y / 3;
                    final_colour_g -= y / 3;
                    final_colour_b -= y / 3;
                }
                else
                {
                    final_colour_r -= (y - last_light_border_y) >> 1;
                    final_colour_g -= (y - last_light_border_y) >> 1;
                    final_colour_b -= (y - last_light_border_y) >> 1;
                }

                // clamp colours
                if (final_colour_r > 255) final_colour_r = 255; 
                if (final_colour_g > 255) final_colour_g = 255;
                if (final_colour_b > 255) final_colour_b = 255;
                if (final_colour_r < 0) final_colour_r = 0; 
                if (final_colour_g < 0) final_colour_g = 0;
                if (final_colour_b < 0) final_colour_b = 0;


                // draw 4x4
                for (uint32_t yy = 0; yy < TILE_SIZE_Y; yy++)
                {
                    for (uint32_t xx = 0; xx < TILE_SIZE_X; xx++)
                    {
                        // >> 2 because 32bpp
                        uint32_t index = (((y + yy) * pitch) + ((x + xx) << 2)) >> 2;

                        uint32_t value = ((currentTile->colour_a & 0xFF) << 24)
                            | ((final_colour_r & 0xFF) << 16) 
                            | ((final_colour_g & 0xFF) << 8)
                            | ((final_colour_b));

                        texture_pixels[index] = value;
                    }
                }

                tile_x++;
            }

            tile_y++;
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
