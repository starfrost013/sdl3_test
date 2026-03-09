// Capy.hpp: Engine level includes
// ONLY INCLUDE TIER0/CORE INCLUDES IN HERE!

#pragma once 

// SDL3
#include <SDL3/SDL.h>

// STL Includes
#include <cstring>
#include <functional>
#include <future>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

// Util Includes
#include <util/logging.hpp>

// Tier0 Includes
#include <core/tier0/memory/memory.hpp>
#include <core/tier0/quadtree.hpp>
#include <core/tier0/vector2.hpp>

// Some base systems that are useful to have everywhere, as long as they don't have any other dependencies,
// or those dependencies can be resolved by using forward declarations
#include <core/cmdline/cmdline.hpp>
#include <core/command/command.hpp>
#include <core/cvar/cvar.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/script/script.hpp>

namespace Capy
{
    #define STRING_EMPTY                    ""
    #define STRING_WHITESPACE_DELIMITERS    " \n\r\t\f\v"
    #define STRING_MAX                      256     // Generic max string length
    #define ARRAY_SIZE(x)                   sizeof(x)/sizeof(x[0])

    #define APP_NAME "Capybara Wars Project - CONFIDENTIAL"
    #define APP_VERSION "Pre-Alpha 8.0"
    #define APP_BUILD_DATE __DATE__ " " __TIME__ 

    #define APP_SIGNON_STRING APP_NAME " " APP_VERSION " " APP_BUILD_DATE
}

