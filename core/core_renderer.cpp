#include <cmath>
#include <iostream>
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"

#include <core/core.hpp>
#include <data/levels/levels_temp.hpp>
#include <data/entities/entity_player.hpp>


// Implements the Digital Differential Analyzer (DDA) algorithm, for the purposes of rendering a level.
void Game_RenderLevel()
{  

    


    SDL_UnlockTexture(game.render_target);
    
    // render the texture
    SDL_RenderTexture(game.renderer, game.render_target, NULL, NULL);
}