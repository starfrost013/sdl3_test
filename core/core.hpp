/* Core SDL init/shutdown code */
#pragma once
#include "SDL3/SDL_scancode.h"
#include <Capy.hpp>

namespace Capy
{
    #define NS_PER_SECOND               1000000000
    #define MAX_STRING_GENERIC          256     // Generic max string length

    /* Game states */
    enum GameState 
    {
        Running = 0,
    };

    /* Game settings */
    struct GameSettings
    {  
        uint32_t screenX;
        uint32_t screenY;
    };

    /* Game information that changes on level change */
    struct GameInfo 
    {
        uint32_t timer;
    };


    struct Game
    {
        SDL_Window* window;         // SDL Window
        SDL_Renderer* renderer;     // SDL Renderer
        SDL_Texture* renderTarget;  // Texture that gets blited to the display
        uint32_t tickrate;          // The game's tickrate
        uint64_t lastTickTime;      // last frame time in nanoseconds (returned by SDL_GetTicksNS)
        GameInfo info;              // Level-specific game information
        GameSettings settings;      // Game settings
        bool running;               // Determines if the game is running.
    };

    extern Game game; 

    // Core functionality
    bool Game_Init();               // Run on init
    void Game_PumpEvents();         // Pump events for SDL
    void Game_Tick();               // Run each tick
    void Game_Frame();              // Run each frame
    bool Game_Shutdown();           // RUn on shutdown

    // scancodes are a terrible idea
    extern bool keyStates[];

    bool Input_KeyIsDown(uint32_t scancode);
}
