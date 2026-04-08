/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

    Currently this just wraps C++ streams but could always be changed alter to something more interesting# 
    Also implements a big file that is good for slow disks
*/

#pragma once
#include <Capy.hpp>
#include <filesystem>

namespace Capy
{
    // Maximum safe path length
    #define MAX_PATH                        260
    
    #define FILESYSTEM_PACKAGE_NAMESPACE    "pak:"                  // Namespace for loading shit from a pak
    #define FILESYSTEM_PACKAGE_EXTENSION    ".beer"                 // BEER
    #define FILESYSTEM_PACKAGE_MAGIC        0x52454542              // 'BEER' (little endian)

    class Cvar;

    extern Cvar* baseDirectory; 

    enum FilesystemFileMode
    {   
        FILE_TEXT = (1 << 0),
        FILE_BINARY = (1 << 1),
    };

    // Package file that all game content is located in
    class FilesystemImage
    {
        struct FilesystemImageHeader
        {
            uint32_t magic;
            uint32_t version;
            uint32_t numFiles;
        };

        struct FilesystemImageFileList
        {
            const char* path; 
            size_t location;
            size_t size; 
        };

        FilesystemImageHeader header; 
        FilesystemImageFileList fileList;

        // next image in the chain
        FilesystemImage* next; 
    };

    // first member of filesystem image chain
    extern FilesystemImage* firstImage;

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

    private: 

        static void OpenImage(const char* path);

    };

};

