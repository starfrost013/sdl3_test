/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

    Currently this just wraps C++ streams but could always be changed alter to something more interesting# 
    Also implements a big file that is good for slow disks
*/

#pragma once
#include <Brewery.hpp>
#include <filesystem>

namespace Brewery
{
    // Maximum safe path length
    #define MAX_PATH                        260
    
    #define FILESYSTEM_PACKAGE_NAMESPACE    "pak:"                  // Namespace for loading shit from a pak
    #define FILESYSTEM_PACKAGE_EXTENSION    ".beer"                 // BEER
    #define FILESYSTEM_PACKAGE_MAGIC        0x52454542              // 'BEER' (little endian)
    #define FILESYSTEM_PACKAGE_VERSION      1                       // Beerfile version

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

        struct FilesystemImageFileEntry
        {
            const char* path; 
            size_t location;
            size_t size; 
        };
  public: 
        FilesystemImageHeader header; 
        std::vector<FilesystemImageFileEntry> fileList;

        std::fstream stream;

        bool Write(const char* path)
        {
            if (!strstr(path, FILESYSTEM_PACKAGE_EXTENSION))
            {
                Logging_LogChannel("Beerfiles must have the .BEER extension", LogChannel::Error);
                return false; 
            }

            stream.open(path, std::ios_base::binary);

            if (stream.bad())
            {
                Logging_LogChannel("Failed to open beerfile: Could not open file", LogChannel::Error);
                return false;
            }

            //ensure the magic is there
            header.magic = FILESYSTEM_PACKAGE_MAGIC;
            header.version = FILESYSTEM_PACKAGE_VERSION;

            stream.write((char*)(&header), sizeof(header));

            // write the image header
            for (FilesystemImageFileEntry entry : fileList)
            {
                stream.write((char*)(&entry), sizeof(FilesystemImageFileEntry));
            }
            
            stream.close();

            return true; 
        }
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
    private: 


    };

};

