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

        engine.settings.screenX = 1024;
        engine.settings.screenY = 768;

        if (!SDL_CreateWindowAndRenderer(APP_SIGNON_STRING, engine.settings.screenX, engine.settings.screenY, 0, &engine.window, &engine.renderer))
            return false;

        Logging_LogChannel("Initialising render texture...", LogChannel::Message);

        // not really a render target, but w/e
        engine.renderTarget = SDL_CreateTexture(engine.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, engine.settings.screenX, engine.settings.screenY);

        if (!engine.renderTarget)
        {
            Logging_LogChannel("Failed to create render target: %s", LogChannel::Error, SDL_GetError());

            Engine_Shutdown();
        }

        return true; 
    }

    void Render_Clear()
    {
        SDL_RenderClear(engine.renderer);

    }

    void Render_Present()
    {
        // flip the buffers
        SDL_RenderPresent(engine.renderer);
    }

    void Render_SetWindowTitle(const char* wndTitle)
    {
        SDL_SetWindowTitle(engine.window, wndTitle);
    }

    void Render_Shutdown()
    {
        SDL_DestroyRenderer(engine.renderer);
        SDL_DestroyWindow(engine.window);

        SDL_Quit();
    }
}
    