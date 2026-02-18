/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

    Currently this just wraps C++ streams but could always be changed alter to something more interesting
*/

#pragma once
#include <Capy.hpp>
#include <filesystem>

namespace Capy
{
    // Maximum safe path length
    #define MAX_PATH                260

    class Cvar;

    extern Cvar* baseDirectory; 

    enum FilesystemFileMode
    {   
        FILE_TEXT = (1 << 0),
        FILE_BINARY = (1 << 1),
    };

    // in the future this will load from a pakcage file
    class FilesystemFile
    {
        friend class Filesystem;

        public:
            std::fstream stream;                // the backing stream of the file
            char path[MAX_PATH];                // the path to the file (full)
            bool open;                          // is the file open?

    };

    class Filesystem
    {
    public: 
    
        //
        // METHODS
        //

        static void Init();

        static FilesystemFile* Open(const char* path, FilesystemFileMode mode = FilesystemFileMode::FILE_TEXT);
        static void Close(FilesystemFile* ff);

    };

    
    
};

