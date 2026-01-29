/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

*/

#include <core/core.hpp>

namespace Capy
{
    class FilesystemSettings
    {        
        friend class Filesystem;

        char baseDirectory[MAX_STRING_GENERIC];
    };

    class FilesystemFile
    {
        friend class Filesystem;

        const char* path;
        bool open;
        std::fstream file;
    };

    class Filesystem
    {
        static FilesystemFile* Open(const char* path);
        static void Close(FilesystemFile* ff);
    };

    
};

