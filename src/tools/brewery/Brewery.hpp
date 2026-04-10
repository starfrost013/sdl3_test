// Breweryhpp: Main header file

#pragma once 

// STL Includes
#include <cassert>
#include <cstring>
#include <functional>
#include <future>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

// Includes
#include <memory/memory.hpp>
#include <util/logging.hpp>
#include <util/util.hpp>
#include <cmdline/cmdline.hpp>
#include <filesystem/filesystem.hpp>

namespace Brewery
{
    #define STRING_EMPTY                    ""
    #define STRING_WHITESPACE_DELIMITERS    " \n\r\t\f\v"
    #define STRING_MAX                      256     // Generic max string length
    #define MAX_PATH                        260     // Damn WinDoze!
    #define ARRAY_SIZE(x)                   sizeof(x)/sizeof(x[0])

    #define APP_NAME "Brewery"
    #define APP_VERSION "1.0.0"
    #define APP_BUILD_DATE __DATE__ " " __TIME__ 

    #define APP_SIGNON_STRING APP_NAME " " APP_VERSION " " APP_BUILD_DATE
}

