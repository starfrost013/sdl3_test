/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

*/

#include <core/core.hpp>

namespace Capy
{
    class FilesystemSettings
    {
        char baseDirectory[MAX_STRING_GENERIC];
    };

    class FilesystemFile
    {
        const char* path;
        bool open;
        std::fstream file;
    };

    class Filesystem
    {
        friend FilesystemFile;
        friend FilesystemSettings;

        static FilesystemFile* Open(const char* path);
        static void Close(FilesystemFile* ff);
    };

    
};

