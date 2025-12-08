#include <cmath>
#include <iostream>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <core/core.hpp>
#include <data/entities/entity_player.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    WorldEntity world;                     // TEMP

    Game game = {0};                // Core game strcture

    bool Game_Init()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
            return false; 

        game.settings.screen_x = 1024;
        game.settings.screen_y = 768;

        if (!SDL_CreateWindowAndRenderer(APP_SIGNON_STRING, game.settings.screen_x, game.settings.screen_y, 0, &game.window, &game.renderer))
            return false;

        // not really a render target, but w/e
        game.render_target = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, game.settings.screen_x, game.settings.screen_y);

        if (!game.render_target)
        {
            std::cout << "Failed to create render target " << SDL_GetError() << std::endl;
            Game_Shutdown();
        }
        
        game.running = true; 
        game.tickrate = 60; 

        world.Create();
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
                    key_state[next_event.key.scancode] = false; 
                    break; 
                case SDL_EVENT_KEY_DOWN:
                    key_state[next_event.key.scancode] = true;
                    break; 
                case SDL_EVENT_QUIT:
                    game.running = false; 
                    break;

            }
        }
    }

    void Game_Tick()
    {
        /* TODO: Move to entity event */

        bool w_down = Input_KeyIsDown(SDL_SCANCODE_W);
        bool a_down = Input_KeyIsDown(SDL_SCANCODE_A);
        bool s_down = Input_KeyIsDown(SDL_SCANCODE_S);
        bool d_down = Input_KeyIsDown(SDL_SCANCODE_D);
        /* 
            move the player around based on their movement speed
            test each axis individually so we can slide along walls 
        */

        uint32_t collision_index = 0;
        bool movement_allowed_x = true; //used for debugging 
        bool movement_allowed_y = true; 
    }
    
    void Game_Frame()
    {
        world.Render();
    }

    bool Game_Shutdown()
    {
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);

        SDL_Quit();

        exit(0); // allow exit codes for errors?

        return true;
    }
}
