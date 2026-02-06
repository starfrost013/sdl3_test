/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

*/

#include <core/core.hpp>
#include <filesystem>

namespace Capy
{
    struct FilesystemSettings
    {        
        friend class Filesystem;

        bool useBaseDirectory;
        char baseDirectory[MAX_STRING_GENERIC];
    };

    extern FilesystemSettings defaultFilesystemSettings;

    class FilesystemFile
    {
        friend class Filesystem;

        const char* path;
        bool open;
        std::fstream file;
    };

    class Filesystem
    {
    public: 
    
        //
        // GLOBALS
        //
        static FilesystemSettings settings;

        //
        // METHODS
        //

        static void Init(FilesystemSettings fsSettings);

        static FilesystemFile* Open(const char* path);
        static void Close(FilesystemFile* ff);

    };

    
    
};

