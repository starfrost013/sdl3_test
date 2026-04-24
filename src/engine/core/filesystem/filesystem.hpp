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
    
    #define FILESYSTEM_PACKAGE_NAMESPACE    "beer!"                 // Namespace for loading shit from a pak
    #define FILESYSTEM_PACKAGE_EXTENSION    ".beer"                 // BEER
    #define FILESYSTEM_PACKAGE_SEPARATOR    ':'                     // COLON
    #define FILESYSTEM_PACKAGE_MAGIC        0x52454542              // 'BEER' (little endian)
    #define FILESYSTEM_PACKAGE_VERSION      1                       // Beerfile version
    #define FILESYSTEM_PACKAGE_CHUNK_SIZE   1048576                 // 1 MB, should be reasonably fast?

    class Cvar;

    extern Cvar* fsBasedir; 

    enum FilesystemFileMode
    {   
        FILE_TEXT = (1 << 0),
        FILE_BINARY = (1 << 1),
    };

    // filesystem file type
    enum FilesystemFileType
    {
        FILETYPE_FS = 0,            // This file is in the real filesystem
        FILETYPE_PAK = 1,           // THis file is in the virtual beer filesystem
    };

        // in the future this will load from a pakcage file
    class FilesystemFile
    {
        friend class Filesystem;

        public:
            std::fstream* stream;               // the backing stream of the file
            char path[MAX_PATH];                // the path to the file (full)
            bool open;                          // is the file open?

        FilesystemFile()
        {
            stream = new std::fstream;
        }

        ~FilesystemFile()
        {
            delete stream;
        }
    };
    
    // Package file that all game content is located in
    class FilesystemImage
    {
    public: 
        struct FilesystemImageHeader
        {
            uint32_t magic;
            uint32_t version;
            uint32_t numFiles;
        };

        struct FilesystemImageFileEntry
        {
            char path[MAX_PATH];
            size_t location;
            size_t size; 

            // stored to make iteration slightly more efficient
            int id; 
        };

        FilesystemImageHeader header; 
        std::vector<FilesystemImageFileEntry> fileList;

        std::fstream* stream;

        // Engine only
        FilesystemImage* next;
        char path[MAX_PATH] = {0};                              // the path 

        bool AddFile(const char* path);                         // Add a file to a beerfile to be written.
        FilesystemImageFileEntry* GetFileByPath(const char* name);  
        bool Read(const char* path);                            // Read a beerfile.
        bool Write(const char* path);                           // Write out a beerfile.
        FilesystemFile* OpenFile(const char* path);             // seek to the start of a file.
        void Close();

        FilesystemImage()
        {
            stream = new std::fstream;   
        }

        ~FilesystemImage()
        {
            delete stream;
        }

    private: 
        bool Open(const char* path);            // open the mage
    };

    // first member of filesystem image chain
    extern FilesystemImage* imageListHead;



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
        static FilesystemFile* OpenInternal(const char* path, FilesystemFileMode mode, FilesystemFileType type);
    };

    // Some helper functions
    FilesystemImage* FS_GetImageByName(const char* name);

};

