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
        Cmdline_Init(argc, argv);
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

        bool success = false; 
        try
        {
            std::filesystem::recursive_directory_iterator fileIterator(image.basedir, std::filesystem::directory_options::skip_permission_denied);

            for (const auto& dirEntry : fileIterator)
            {
                success = image.AddFile(reinterpret_cast<const char*>(dirEntry.path().c_str())); // bad idea
            
                if (!success)
                    goto exit_failure;
                    
                numFiles++;
            }
        }
        catch(const std::filesystem::filesystem_error& e)
        {
            Logging_LogChannel("Failed to create image: %s", LogChannel::Error, e.what());
            goto exit_failure; 
        }

        image.Write(Cmdline_Argv(2));
        return EXIT_SUCCESS;
    
    exit_failure:

        return EXIT_FAILURE;
    }
}

int32_t main(int32_t argc, char** argv)
{
    return Brewery::Brewery_Main(argc, argv);
}