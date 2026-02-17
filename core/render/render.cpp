#include <core/render/render.hpp>

namespace Capy
{
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
}
    