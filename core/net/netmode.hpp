/*
    Capybara Wars © 2025, 2026 

    NetModeBase.hpp: Base class for netowrk modes.
*/

#pragma once
#include <core/core.hpp>

namespace Capy 
{
    class NetMode
    {
        public:
            virtual void Init() = 0;
            virtual void Shutdown() = 0;
    };
}
