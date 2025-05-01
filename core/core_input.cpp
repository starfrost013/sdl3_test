#include <cmath>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <core/core.hpp>

/* A pretty basic input system */

bool key_state[SDL_SCANCODE_COUNT];

bool Input_KeyIsDown(uint32_t scancode)
{
    return key_state[scancode];
}
