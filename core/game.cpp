#include <cmath>
#include <iostream>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "util/logging.hpp"
#include <Capy.hpp>

#include <core/net/net.hpp>
#include <core/net/client/client.hpp>
#include <core/net/server/server.hpp>
#include <core/game.hpp>
#include <data/entities/entity_player.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    WorldEntity world;              // TEMP

    Game game;                      // Core game strcture
    Client* client = nullptr;       // The client    
    Server* server = nullptr;       // The server

    // These Render_* functions are temporary until we have a real render layer with multiple renderers
    bool Render_Init()
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

        return true; 
    }

    void Render_Clear()
    {
        SDL_RenderClear(game.renderer);

    }

    void Render_Present()
    {
        // flip the buffers
        SDL_RenderPresent(game.renderer);
    }

    void Render_Shutdown()
    {
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);

        SDL_Quit();
    }

    bool Game_Init(int32_t argc, char** argv)
    {
        logger.settings.channels = (LogChannel)(LogChannel::Debug | LogChannel::Message | LogChannel::Warning | LogChannel::Error | LogChannel::Fatal | LogChannel::SuperFatal);
        logger.settings.destination = (LogDestination)(LogDestination::File | LogDestination::Printf);
        logger.settings.keepOldLogs = false;
#ifdef RELEASE // don't need to ifdef debug because it gets logged
        logger.settings.destination = (LogDestination)(LogDestination::File);
#endif

        if (!Logging_Init())
            std::cout << "Catastrophic non-fatal error: SSLS Logger Initialisation FAILED (0xDEADDEAD). You won't get any logging!\n" << std::endl;            
#ifdef __linux__
        Logging_LogChannel("64-bit Linux binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_LINUX64;
#elif _WIN32
        Logging_LogChannel("64-bit Windows binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_WIN64;
#endif
        Cmdline_Init(argc, argv);
        CapyNet_Init();

        NetType mode = static_cast<NetType>(int(netMode->value));

        switch (mode)
        {
            case NETMODE_CLIENT:
                Logging_LogChannel("This is a client", LogChannel::Message);
                client = new Client();
                break;
            // TODO: Listen servers
            case NETMODE_SERVER_LISTEN:
                Logging_LogChannel("This is a listen server", LogChannel::Message);
                client = new Client();
                server = new Server(PORT_DEFAULT);
                break;
            case NETMODE_SERVER_DEDICATED:
                Logging_LogChannel("This is a dedicated server", LogChannel::Message);
                server = new Server(PORT_DEFAULT);
                break; 
        }
        
        if (client)
            client->Init();
        
        if (server)
            server->Init();

        if (mode != NETMODE_SERVER_DEDICATED)
        {
            if (!Render_Init())
            {
                // error will already be printed
                Game_Shutdown();
                return false;
            }

            world.GetHeader().SetSize(Vector2(3000, 400));

            world.Create();
        }

        game.running = true; 
        game.tickrate = 60; 

        if (mode == NETMODE_CLIENT)
        {
            client->Connect("82.37.141.130");    
        }

        return true; 
    }

    void Game_Run()
    {
        NetType mode = static_cast<NetType>(int(netMode->value));

        /* Update the game world, using nanoseconds for more precision */
        uint64_t time_now = SDL_GetTicksNS();

        /* Now, pump the game's event queue */
        Game_PumpEvents();

        if (time_now > (game.lastTickTime + (NS_PER_SECOND / game.tickrate)))
        {
            /* Update the world state and actually *HANDLE* those events */
            Game_Tick();
            //std::cout << "Last tick time: " << (float(time_now / 1000000.0f)) - (float(game.last_tick_time / 1000000.0f)) << "ms" << std::endl;
            game.lastTickTime = time_now;
        }

        if (mode != NETMODE_SERVER_DEDICATED)   
        {
            Game_Frame();
        }
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
        if (client)
            client->Tick();

        if (server)
            server->Tick();
    }
    
    void Game_Frame()
    {            
        // update shared first
        Render_Clear();

        world.Render();

        if (client)
            client->Frame();

        if (server)
            server->Frame();

        Render_Present();
    }

    bool Game_Shutdown()
    {
        CapyNet_Shutdown();
        Cvar_Shutdown();
        
        Render_Shutdown();

        Logging_Shutdown();

        exit(0); // allow exit codes for errors?

        return true;
    }
}
