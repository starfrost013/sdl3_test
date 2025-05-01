#include <cmath>
#include <iostream>
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"

#include <core/core.hpp>
#include <data/levels/levels_temp.hpp>
#include <entities/entity_player.hpp>

/* Set the level ID for the game */
void Game_SetLevel(uint32_t level_id)
{
    if (level_id >= NUM_LEVELS)
    {
        //temp
        std::cout << "Invalid level selected" << std::endl;
        Game_Shutdown();
        exit(1);
    }

    game.info.level_id = level_id; 
}

// Implements the Digital Differential Analyzer (DDA) algorithm, for the purposes of rendering a level.
void Game_RenderLevel()
{  
    uint8_t* level_data = level_temp_01;

    if (game.info.level_id == 1)
        level_data = level_temp_02;

    uint32_t size_x = level_data[LEVEL_HEADER_SIZE_X];
    uint32_t size_y = level_data[LEVEL_HEADER_SIZE_Y];

    /* prevent drawing random fucking memory as the fucking level */
    if (player.x >= size_x
    || player.y >= size_y
    || player.x < 0
    || player.y < 0)
        return;

    // float should be sufficient for what we need 
    for (int32_t x = 0; x < game.settings.screen_x; x++)
    {
        float camera_x = 2 * x / float(game.settings.screen_x) - 1;
        //direction to send the rays in
        float ray_direction_x = player.dir_x + player.plane_x * camera_x;
        float ray_direction_y = player.dir_y + player.plane_y * camera_x;
        
        // position int he map we are in 
        int32_t map_position_x = player.x, map_position_y = player.y;

        float delta_dist_x = (ray_direction_x == 0) ? 1e30 : std::abs(1 / ray_direction_x);
        float delta_dist_y = (ray_direction_y == 0) ? 1e30 : std::abs(1 / ray_direction_y);

        // set our step directions
        int32_t step_x = (ray_direction_x < 0) ? -1 : 1;
        int32_t step_y = (ray_direction_y < 0) ? -1 : 1;

        float side_dist_x = 0.0f, side_dist_y = 0.0f; 

        // set side distance
        if (ray_direction_x < 0)
            side_dist_x = (player.x - map_position_x) * delta_dist_x;
        else
            side_dist_x = ((map_position_x + 1.0f) - player.x) * delta_dist_x;

        if (ray_direction_y < 0)
            side_dist_y = (player.y - map_position_y) * delta_dist_y;
        else
            side_dist_y = ((map_position_y + 1.0f) - player.y) * delta_dist_y;

        bool hit = false; 
        // store the id of which side we hit (0 = left/right, 1 = top/bottom)
        int32_t side_hit = 0;

        int32_t world_position = 0; // final piece of level to draw

        // Perform a DDA
        while (!hit)
        {
            if (side_dist_x < side_dist_y)
            {
                side_dist_x += delta_dist_x;
                map_position_x += step_x;
                side_hit = 0;
            }
            else
            {
                side_dist_y += delta_dist_y;
                map_position_y += step_y;
                side_hit = 1;
            }

            // figure out the world map position. Add two for the header size
            world_position = (size_y * map_position_y) + map_position_x + LEVEL_HEADER_SIZE;

            // is there a non-air block there. if so, we hit
            if (level_data[world_position] > 0)
                hit = true; 
        }

        // Calculate the perpendicular distance to the wall 
        float perp_wall_dist = (side_hit == 0) ? (side_dist_x - delta_dist_x) : (side_dist_y - delta_dist_y);

        /* figure out how big the wall is */
        int32_t line_height = game.settings.screen_y / perp_wall_dist;
        int32_t draw_start = -line_height / 2 + game.settings.screen_y / 2;

        if (draw_start < 0)
            draw_start = 0;

        int32_t draw_end = line_height / 2 + game.settings.screen_y / 2;

        if (draw_end > game.settings.screen_y)
            draw_end = game.settings.screen_y - 1;

        if (level_data[world_position] > 0)
        {
            uint8_t r = 60;
            uint8_t g = 170;
            uint8_t b = 40; 

            // draw darker if y

            if (side_hit == 1)
            {
                //halve r and b and slightly reduce g
                r <<= 1;
                g = 140;
                b <<= 1; 
            }

            SDL_SetRenderDrawColor(game.renderer, r, g, b, 255);
            SDL_RenderLine(game.renderer, x, draw_start, x, draw_end);
            SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 0);

        }

    }

}