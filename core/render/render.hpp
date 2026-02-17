//
// Temporary render system
// These Render_* functions are temporary until we have a real render layer with multiple renderers
//

#pragma once

#include <core/game.hpp>

namespace Capy
{
    bool Render_Init();                 // Initialises the renderer
    void Render_Clear();            
    void Render_Present();
    void Render_Shutdown();
}
