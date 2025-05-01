#include <cmath>
#include <iostream>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <core/core.hpp>
#include <entities/entity_player.hpp>


Game game = {0};                // Core game strcture

bool Game_Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
        return false; 

    game.settings.screen_x = 1024;
    game.settings.screen_y = 768;

    if (!SDL_CreateWindowAndRenderer(APP_SIGNON_STRING, game.settings.screen_x, game.settings.screen_y, 0, &game.window, &game.renderer))
        return false;
    
    game.running = true; 
    game.tickrate = 60; 

    /* temp player init code */
    player.x = 3;
    player.y = 3;
    
    // works for both
    player.spawn_x = 3;
    player.spawn_y = 3;

    // set the camera plane
    player.plane_x = 0; 
    player.plane_y = 0.66;

    player.dir_x = -1;

    player.move_speed = 0.09;
    player.rotation_speed = 0.09; // radians

    return true; 
}

void Game_PumpEvents()
{
    SDL_Event next_event; 

    if (SDL_PollEvent(&next_event))
    {
        bool w_down = false, a_down = false, s_down = false, d_down = false;
        bool left_down = false, right_down = false, up_down = false, down_down = false;  

        switch (next_event.type)
        {
            /* TEMP code */
            case SDL_EVENT_KEY_UP:
                key_is_down[next_event.key.scancode] = false; 
                break; 
            case SDL_EVENT_KEY_DOWN:
                key_is_down[next_event.key.scancode] = true;
                 
                break; 
            case SDL_EVENT_QUIT:
                game.running = false; 
                break;

        }
    }
}

void Game_Tick()
{
    bool w_down = Input_KeyIsDown(SDL_SCANCODE_W);
    bool a_down = Input_KeyIsDown(SDL_SCANCODE_A);
    bool s_down = Input_KeyIsDown(SDL_SCANCODE_S);
    bool d_down = Input_KeyIsDown(SDL_SCANCODE_D);

    /* move the player around based on their movement speed */
    if (w_down) 
    {
        player.x += player.dir_x * player.move_speed;
        player.y += player.dir_y * player.move_speed;
    }

    if (s_down) 
    {
        player.x -= player.dir_x * player.move_speed;
        player.y -= player.dir_y * player.move_speed;
    }
    
    // rotate using some basic trig
    if (d_down)
    {
        float old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(-player.rotation_speed) - player.dir_y * sin(-player.rotation_speed);
        player.dir_y = old_dir_x * sin(-player.rotation_speed) + player.dir_y * cos(-player.rotation_speed);

        float old_plane_x = player.plane_x;
        player.plane_x = player.plane_x * cos(-player.rotation_speed) - player.plane_y * sin(-player.rotation_speed);
        player.plane_y = old_plane_x * sin(-player.rotation_speed) + player.plane_y * cos(-player.rotation_speed);

    }
        
    if (a_down)
    {
        float old_dir_x = player.dir_x;
        player.dir_x = player.dir_x * cos(player.rotation_speed) - player.dir_y * sin(player.rotation_speed);
        player.dir_y = old_dir_x * sin(player.rotation_speed) + player.dir_y * cos(player.rotation_speed);

        float old_plane_x = player.plane_x;
        player.plane_x = player.plane_x * cos(player.rotation_speed) - player.plane_y * sin(player.rotation_speed);
        player.plane_y = old_plane_x * sin(player.rotation_speed) + player.plane_y * cos(player.rotation_speed);
    }

    if (w_down || a_down || s_down || d_down)
    {
        std::cout << "X: " << player.x << " Y: " << player.y << std::endl;
    }
}

bool Game_Shutdown()
{
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);

    SDL_Quit();

    return true;
}