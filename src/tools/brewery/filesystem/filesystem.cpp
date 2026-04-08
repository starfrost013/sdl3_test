#include <filesystem/filesystem.hpp>

namespace Brewery
{
    //
    // GLOBALS
    //

    // first member of filesystem image chain
    FilesystemImage* firstImage;
    
    //
    // METHODS
    //

    void Filesystem::Init()
    {

    }

    void Filesystem::OpenImage(const char* path)
    {
        Logging_LogChannel("Mounting beer image... %s", LogChannel::Message, path); 

    }

    void Filesystem::CloseImage(FilesystemImage* image)
    {
    
    }
};