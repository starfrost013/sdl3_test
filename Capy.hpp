// Capy.hpp: Engine level includes
// ONLY INCLUDE TIER0/CORE INCLUDES IN HERE!

#pragma once 
#include <SDL3/SDL.h>

// Util Includes
#include <util/logging.hpp>

// Tier0 Includes
#include <core/tier0/memory/memory.hpp>
#include <core/tier0/quadtree.hpp>
#include <core/tier0/vector2.hpp>

// Some base systems that are useful to have everywhere
#include <core/cmdline/cmdline.hpp>
#include <core/cvar/cvar.hpp>

#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace Capy
{
    #define STRING_EMPTY                ""
    #define STRING_MAX_GENERIC          256     // Generic max string length

    #define APP_NAME "Capybara Wars Project - CONFIDENTIAL"
    #define APP_VERSION "Pre-Alpha 5.0"
    #define APP_BUILD_DATE __DATE__ " " __TIME__ 

    #define APP_SIGNON_STRING APP_NAME " " APP_VERSION " " APP_BUILD_DATE
}

