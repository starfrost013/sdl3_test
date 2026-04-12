/* 
    Filesystem core code
    These classes are prefixed with "Capy" to avoid clashing with std::filesystem

    Currently this just wraps C++ streams but could always be changed alter to something more interesting# 
    Also implements a big file that is good for slow disks
*/

#pragma once
#include <Brewery.hpp>

namespace Brewery
{
    // Maximum safe path length
    #define MAX_PATH                        260
    
    #define FILESYSTEM_PACKAGE_NAMESPACE    "pak:"                  // Namespace for loading shit from a pak
    #define FILESYSTEM_PACKAGE_EXTENSION    ".beer"                 // BEER
    #define FILESYSTEM_PACKAGE_MAGIC        0x52454542              // 'BEER' (little endian)
    #define FILESYSTEM_PACKAGE_VERSION      1                       // Beerfile version
    #define FILESYSTEM_PACKAGE_CHUNK_SIZE   1048576                 // 1 MB, should be reasonably fast?

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

            // get theLAST index of the basedir 
            // assume the basedir is at the start....
            char* strBasedir = strstr(entry.path, basedir);
            char* found = NULL; 
            while ((strBasedir = strstr(strBasedir, basedir)) != NULL)
                found = strBasedir++;
     
            size_t pathLength = strlen(entry.path);
            size_t basedirPosition = found - entry.path;
            size_t basedirLength = pathLength - basedirPosition;

            strncpy(entry.path, entry.path + basedirPosition, basedirLength);
            
            entry.path[pathLength - basedirPosition] = '\0';

            fileList.push_back(entry);

            return true; 
        }

        bool Write(const char* path)
        {
            if (!path)
            {
                Logging_LogChannel("Beerfiles must have a path!", LogChannel::Error);
                return false;
            }

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

            std::fstream tempStream; 

            // allocate the temporary buffer

            uint8_t* fileBuf = new uint8_t[FILESYSTEM_PACKAGE_CHUNK_SIZE];

            // TODO; WRITE FILES 
            for (FilesystemImageFileEntry entry : fileList)
            {
                // get the full path of the file
                // needs to be * 2 because two max_path strings could theoretically be in here.
                // TODO: truncate( better things to do)
                char realFileName[MAX_PATH * 2] = {0};

                snprintf(realFileName, MAX_PATH * 2, "%s%s", basedir, entry.path);

                tempStream.open(realFileName, std::ios_base::binary);
                
                if (tempStream.bad())
                {
                    Logging_LogChannel("Cannot write image: Failed to open %s", LogChannel::Debug, realFileName);
                    delete[] fileBuf;
                    return false; 
                } 

                while (!tempStream.eof())
                {
                    tempStream.read((char*)fileBuf, FILESYSTEM_PACKAGE_CHUNK_SIZE);
                    auto gcount = tempStream.gcount();
                    stream.write((char*)fileBuf, gcount); // only write what was really written so we don't get junk from older files in there
                
                    Logging_LogChannel("Wrote %d bytes", LogChannel::Debug, gcount);
                }

                tempStream.close();
            }

            delete[] fileBuf; 

            stream.close();
            return true;
        }
    };
};

