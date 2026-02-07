#include <cmath>
#include <iostream>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "util/logging.hpp"
#include <core/core.hpp>
#include <data/entities/entity_player.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    WorldEntity world;                     // TEMP

    Game game = {0};                // Core game strcture

    bool Game_Init()
    {
        logger.settings.channels = (LogChannel)(LogChannel::Debug | LogChannel::Message | LogChannel::Warning | LogChannel::Error | LogChannel::Fatal | LogChannel::SuperFatal);
        logger.settings.destination = (LogDestination)(LogDestination::File | LogDestination::Printf);
        logger.settings.keepOldLogs = false;
#ifdef RELEASE // don't need to ifdef debug because it gets logged
        logger.settings.destination = (LogDestination)(LogDestination::File);
#endif

        if (!Logging_Init())
            std::cout << "Catastrophic non-fatal error: SSLS Logger Initialisation FAILED (0xDEADDEAD). You won't get any logging!\n" << std::endl;            

        Logging_LogChannel("Initialising SDL...", LogChannel::Message);

#ifdef __linux__
        Logging_LogChannel("64-bit Linux binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_LINUX64;
#elif _WIN32
        Logging_LogChannel("64-bit Windows binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_WIN64;
#endif
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
                    keyStates[next_event.key.scancode] = false; 
                    break; 
                case SDL_EVENT_KEY_DOWN:
                    keyStates[next_event.key.scancode] = true;
                    break; 
                case SDL_EVENT_QUIT:
                    game.running = false; 
                    break;

            }
        }
    }

    void Game_Tick()
    {
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
