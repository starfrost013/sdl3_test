#include <core/core.hpp>
#include <data/entities/entity.hpp>
#include <data/entities/entity_world.hpp>

namespace Capy
{
    void WorldEntity::Create()
    {

    }

    void WorldEntity::Render()
    {
        // world generation test code

        uint32_t* texture_pixels;  
        int32_t pitch; 

        SDL_Rect new_rect;

        new_rect.x = new_rect.y = 0;
        new_rect.w = game.settings.screen_x;
        new_rect.h = game.settings.screen_y;

        SDL_LockTexture(game.render_target, &new_rect, (void**)&texture_pixels, &pitch);

        // size is simply screen size for now

        SDL_UnlockTexture(game.render_target);
    }

    void WorldEntity::Tick()
    {

    }

    void WorldEntity::Destroy()
    {

    }
}
