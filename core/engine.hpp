/* Core SDL init/shutdown code */
#pragma once

#include "SDL3/SDL_scancode.h"
#include <Capy.hpp>

class Client;
class Server;

namespace Capy
{
    #define NS_PER_SECOND               1000000000

    /* Game target platform */
    enum EnginePlatforms
    {
        OS_WIN64 = 0,                   // 64-bit Microsoft Windows
        OS_LINUX64 = 1,                 // 64-bit Linux
    };

    /* Game states */
    enum GameState 
    {
        Running = 0,
    };

    /* Game settings */
    struct EngineSettings
    {  
        uint32_t screenX;
        uint32_t screenY;
    };

    /* Game information that changes on level change */
    struct EngineInfo 
    {
        uint32_t timer;
        EnginePlatforms targetPlatform;  // target platform
    };

    class Engine
    {
    public:
        SDL_Window* window;         // SDL Window
        SDL_Renderer* renderer;     // SDL Renderer
        SDL_Texture* renderTarget;  // Texture that gets blited to the display
        uint32_t tickrate;          // The game's tickrate
        uint64_t lastTickTime;      // last frame time in nanoseconds (returned by SDL_GetTicksNS)
        EngineInfo info;            // Level-specific game information
        EngineSettings settings;    // Game settings
        bool running;               // Determines if the game is running.
    };
    
    extern Client* client;          // Valid if netmode is LISTEN_SERVER or CLIENT
    extern Server* server;          // Valid if netmode is LISTEN_SERVER Or DEDICATED_SERVER
    extern Engine engine;           // Always valid, stuff shared between client and server

    // Core functionality
    bool Engine_Init(int32_t argc, char** argv);               // Run on init
    void Engine_Run();              // Run while game is running
    void Engine_PumpEvents();       // Pump events for SDL
    void Engine_Tick();             // Run each tick
    void Engine_Frame();            // Run each frame
    bool Engine_Shutdown();         // RUn on shutdown


    // scancodes are a terrible idea
    extern bool keyStates[];

    bool Input_KeyIsDown(uint32_t scancode);
}
