/* Entry point and main function for Brewery */
#include <Brewery.hpp>


#include <iostream>

namespace Brewery
{
    void Brewery_Help()
    {
        Logging_LogChannel("Help:", LogChannel::Message);
        Logging_LogChannel("Brewery <input folder> <destination beerfile> args", LogChannel::Message);
    }

    bool Brewery_AddFile(const char* file)
    {
        if (!std::filesystem::exists(file))
        {
            Logging_LogChannel("The file %s does not exist!", LogChannel::Warning);
            return false;
        }
        std::fstream fileStream;

        
        return true;
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