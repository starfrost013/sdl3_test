/* Entry point and main function for Brewery */

#include "util/logging.hpp"
#include <Brewery.hpp>

#include <iostream>

namespace Brewery
{
    void Brewery_Main(int32_t argc, char** argv)
    {
        Logging_Init();
        
        Logging_LogChannel(APP_SIGNON_STRING, LogChannel::Debug);
    }
}

int32_t main(int32_t argc, char** argv)
{
    Brewery::Brewery_Main(argc, argv);
}