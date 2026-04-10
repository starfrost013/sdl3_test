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
        };

        FilesystemImageHeader header; 
        std::vector<FilesystemImageFileEntry> fileList;

        std::fstream stream;

        //
        // Brewery only
        //
        char basedir[MAX_PATH];

        bool AddFile(const char* path)
        {
            if (!std::filesystem::exists(path))
            {
                Logging_LogChannel("The file %s does not exist!", LogChannel::Warning);
                return false;
            }

            FilesystemImage::FilesystemImageFileEntry entry; 
      
            strncpy(entry.path, path, MAX_PATH);
            entry.size = std::filesystem::file_size(path);
            //entry.location is determined later

             // get the index of the basedir 
            const char* strBasedir = strstr(entry.path, basedir);

            size_t initialLength = strlen(entry.path);
            size_t basedirLength = strlen(strBasedir);
            size_t basedirPosition = strBasedir - entry.path;
            size_t remainingCharacters = strlen(entry.path) - basedirPosition + basedirLength;

            strncpy(entry.path, entry.path + basedirPosition + basedirLength, remainingCharacters);

            //assume it starts with the basedir...would bew eird if it didn't
            if ((basedirLength + basedirPosition) < MAX_PATH)
                entry.path[initialLength - (basedirLength + basedirPosition)] = '\0';

      
            fileList.push_back(entry);

            return true; 
        }

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

            // TODO; WRITE FILES 

            return true; 
        }
    };
};

