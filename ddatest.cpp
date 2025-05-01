/* "Wolfenstein 3D" styled Digital Differential Analyzer raycaster (Test Project for SDL3) */

#include "SDL3/SDL_timer.h"
#include "core/core.hpp"
#include <ddatest.hpp>
#include <core/core.hpp>
#include <iostream>


int32_t main(int32_t argc, char** argv)
{
    Game_Init();

    while (game.running)
    {
        /* Update the game world, using nanoseconds for more precision */
        uint64_t time_now = SDL_GetTicksNS();

        /* Now, pump the game's event queue */
        Game_PumpEvents();

        if (time_now > (game.last_tick_time + (1000000000 / game.tickrate)))
        {
            /* Update the world state and actually *HANDLE* those events */
            Game_Tick();
            //std::cout << "Last tick time: " << (float(time_now / 1000000.0f)) - (float(game.last_tick_time / 1000000.0f)) << "ms" << std::endl;
            game.last_tick_time = time_now; 
            
        }

        SDL_RenderClear(game.renderer);

        Game_RenderLevel();

        // flip the buffers
        SDL_RenderPresent(game.renderer);
    }

    Game_Shutdown();

    return 0; 
}