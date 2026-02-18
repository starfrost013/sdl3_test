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
    class Cvar;
    
    extern Cvar* baseDirectory; 

    class FilesystemFile
    {
        friend class Filesystem;

        public:
            std::fstream stream;                // the backing stream of the file

        private: 
            const char* path;
            bool open;
    };

    class Filesystem
    {
    public: 
    
        //
        // METHODS
        //

        static void Init();

        static FilesystemFile* Open(const char* path);
        static void Close(FilesystemFile* ff);

    };

    
    
};

