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
        Logging_LogChannel("Initialising SDL...", LogChannel::Message);

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
            return false; 

        Logging_LogChannel("Initialising window and renderer...", LogChannel::Message);

        game.settings.screenX = 1024;
        game.settings.screenY = 768;

        if (!SDL_CreateWindowAndRenderer(APP_SIGNON_STRING, game.settings.screenX, game.settings.screenY, 0, &game.window, &game.renderer))
            return false;

        Logging_LogChannel("Initialising render texture...", LogChannel::Message);

        // not really a render target, but w/e
        game.renderTarget = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, game.settings.screenX, game.settings.screenY);

        if (!game.renderTarget)
        {
            Logging_LogChannel("Failed to create render target: %s", LogChannel::Error, SDL_GetError());

            Game_Shutdown();
        }
        
        game.running = true; 
        game.tickrate = 60; 

        world.GetHeader().SetSize(Vector2(3000, 400));

        world.Create();
        return true; 
    }

    void Game_PumpEvents()
    {
        SDL_Event next_event; 

        if (SDL_PollEvent(&next_event))
        {
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

        bool wDown = Input_KeyIsDown(SDL_SCANCODE_W);
        bool aDown = Input_KeyIsDown(SDL_SCANCODE_A);
        bool sDown = Input_KeyIsDown(SDL_SCANCODE_S);
        bool dDown = Input_KeyIsDown(SDL_SCANCODE_D);
        /* 
            move the player around based on their movement speed
            test each axis individually so we can slide along walls 
        */

        uint32_t collisionIndex = 0;
        bool movementAllowedX = true; //used for debugging 
        bool movementAllowedY = true; 
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
