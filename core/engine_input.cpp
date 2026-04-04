#include <cmath>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <Capy.hpp>

namespace Capy
{
    /* A pretty basic input system */

    bool keyStates[SDL_SCANCODE_COUNT];

    bool Input_KeyIsDown(uint32_t scancode)
    {
        return keyStates[scancode];
    }

}

