/* Entry point and main function for Brewery */
#include "cmdline/cmdline.hpp"
#include <Brewery.hpp>


#include <iostream>

namespace Brewery
{
    void Brewery_Help()
    {
        Logging_LogChannel("Help:", LogChannel::Message);
        Logging_LogChannel("Brewery <input folder> <destination beerfile> args", LogChannel::Message);
    }


    int32_t Brewery_Main(int32_t argc, char** argv)
    {
        Logging_Init();
        
        Logging_LogChannel(APP_SIGNON_STRING, LogChannel::Debug);

        if (Cmdline_Argc() < 2)
        {
            Logging_LogChannel("At least a source folder and destination beerfile must be provided!", LogChannel::Error);
            Brewery_Help();

            return EXIT_FAILURE;
        }

        
        FilesystemImage image; 

        // copy the base dir
        strncpy(image.basedir, Cmdline_Argv(1), MAX_PATH);

        image.header.magic = FILESYSTEM_PACKAGE_MAGIC;
        image.header.version = FILESYSTEM_PACKAGE_VERSION;
        
        uint32_t numFiles = 0;

        

        return EXIT_SUCCESS;
    }
}

int32_t main(int32_t argc, char** argv)
{
    return Brewery::Brewery_Main(argc, argv);
}