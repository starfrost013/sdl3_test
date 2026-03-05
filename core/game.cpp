#include <cmath>
#include <iostream>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "util/logging.hpp"
#include <Capy.hpp>

#include <core/filesystem/filesystem.hpp>
#include <net/net.hpp>
#include <net/client/client.hpp>
#include <net/server/server.hpp>
#include <core/game.hpp>
#include <entity/entity_player.hpp>
#include <world/world.hpp>

namespace Capy
{

    Game game;                      // Core game strcture
    Client* client = nullptr;       // The client    
    Server* server = nullptr;       // The server

    void Game_PrintBuildInfo()
    {
        Logging_LogChannel(APP_SIGNON_STRING, LogChannel::Message);

#ifdef __linux__
        Logging_LogChannel("64-bit Linux binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_LINUX64;
#elif _WIN32
        Logging_LogChannel("64-bit Windows binary", LogChannel::Message);
        game.info.targetPlatform = GameTargetPlatform::OS_WIN64;
#endif

#ifdef DEBUG
        Logging_LogChannel("Debug build", LogChannel::Message);
#else
        Logging_LogChannel("Release build", LogChannel::Message);
#endif
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

        Game_PrintBuildInfo();
        Cmdline_Init(argc, argv);
        Command_Init();
        Filesystem::Init();  
        Script_Init(); 
        CapyNet_Init();

        NetModeEnum mode = static_cast<NetModeEnum>(int(netMode->value));

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
                server = new Server(netPort->value);
                break;
            case NETMODE_SERVER_DEDICATED:
                Logging_LogChannel("This is a dedicated server", LogChannel::Message);
                server = new Server(netPort->value);
                break; 
        }
        
        if (client)
            client->Init();
        
        if (server)
            server->Init();

        game.running = true; 
        game.tickrate = 60; 

        if (mode == NETMODE_CLIENT)
        {
            client->Connect(netServerAddress->string);    
        }

        return true; 
    }

    void Game_Run()
    {
        NetModeEnum mode = static_cast<NetModeEnum>(int(netMode->value));

        /* Update the game world, using nanoseconds for more precision */
        uint64_t timeNow = SDL_GetTicksNS();

        /* Now, pump the game's event queue */
        Game_PumpEvents();

        if (timeNow > (game.lastTickTime + (NS_PER_SECOND / game.tickrate)))
        {
            /* Update the world state and actually *HANDLE* those events */
            Game_Tick();
            //std::cout << "Last tick time: " << (float(time_now / 1000000.0f)) - (float(game.last_tick_time / 1000000.0f)) << "ms" << std::endl;
            game.lastTickTime = timeNow;
        }

        Game_Frame();
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
        if (client)
            client->Frame();

        if (server)
            server->Frame();
    }

    bool Game_Shutdown()
    {
        if (client)
            client->Shutdown();

        if (server)
            server->Shutdown();

        CapyNet_Shutdown();
        Script_Shutdown();
        Cvar_Shutdown();

        Logging_Shutdown();

        SDL_Quit();

        exit(0); // allow exit codes for errors?

        return true;
    }
}
