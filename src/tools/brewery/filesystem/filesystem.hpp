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

    #define FILESYSTEM_PACKAGE_NAMESPACE    "beer!"                 // Namespace for loading shit from a pak
    #define FILESYSTEM_PACKAGE_EXTENSION    ".beer"                 // BEER
    #define FILESYSTEM_PACKAGE_SEPARATOR    ':'                     // COLON
    #define FILESYSTEM_PACKAGE_MAGIC        0x52454542              // 'BEER' (little endian)
    #define FILESYSTEM_PACKAGE_VERSION      1                       // Beerfile version
    #define FILESYSTEM_PACKAGE_CHUNK_SIZE   1048576                 // 1 MB, should be reasonably fast?


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
      
            entry.size = std::filesystem::file_size(path);

            //first copy in the path including basedir
            strncpy(entry.path, path, MAX_PATH);

            // find the end of the basedir
            size_t entryLength = strlen(entry.path);
            size_t basedirLength = strlen(basedir);

            size_t endOfBasedirPos = 0;

            for (endOfBasedirPos = 0; endOfBasedirPos < entryLength; endOfBasedirPos++)
            {
                // don't start comparing garbage
                if (endOfBasedirPos >= basedirLength)
                    break;

                if (entry.path[endOfBasedirPos] != basedir[endOfBasedirPos])
                    break;

                endOfBasedirPos++;
            }

            //ensure there are no leading slashes
            if (entry.path[endOfBasedirPos] == '\\'
            || entry.path[endOfBasedirPos] == '/')
            {
                endOfBasedirPos++;
            }

            auto newStringLength = entryLength - endOfBasedirPos;
            strncpy(entry.path, entry.path + endOfBasedirPos, newStringLength);

            // reinsert the trailing zero
            entry.path[newStringLength] = '\0';

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

            stream.open(path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

            if (stream.bad())
            {
                Logging_LogChannel("Failed to open beerfile: Could not open file", LogChannel::Error);
                return false;
            }

            //ensure the magic is there
            header.magic = FILESYSTEM_PACKAGE_MAGIC;
            header.version = FILESYSTEM_PACKAGE_VERSION;

            stream.write((char*)(&header), sizeof(header));

            size_t currentLocation = 0;

            // write the image header
            for (FilesystemImageFileEntry entry : fileList)
            {
                // only write the actual length of things
                stream.write((char*)(&entry.size), sizeof(size_t));
                // now write the location
                entry.location = currentLocation;
                currentLocation += entry.size;
                stream.write((char*)(&entry.location), sizeof(size_t));
                stream.write((char*)(&entry.path), strlen(entry.path) + 1); // include the null terminator. should we change this
            }
            
            std::fstream tempStream; 

            // allocate the temporary buffer

            char* fileBuf = new char[FILESYSTEM_PACKAGE_CHUNK_SIZE];

            // TODO; WRITE FILES 
            for (FilesystemImageFileEntry entry : fileList)
            {
                // get the full path of the file
                // needs to be * 2 because two max_path strings could theoretically be in here.
                // TODO: truncate( better things to do)
                char realFileName[MAX_PATH * 2] = {0};

                // we aget rid of the thing earlier so insert the correct path separator
                snprintf(realFileName, MAX_PATH * 2, "%s%c%s", basedir, std::filesystem::path::preferred_separator, entry.path);

                tempStream.open(realFileName, std::ios_base::in | std::ios_base::binary);
                
                if (tempStream.bad())
                {
                    Logging_LogChannel("Cannot write image: Failed to open %s", LogChannel::Debug, realFileName);
                    delete[] fileBuf;
                    return false; 
                } 

                auto gcount = 0;

                while (!tempStream.eof()
                && !tempStream.fail())
                {
                    tempStream.read(fileBuf, FILESYSTEM_PACKAGE_CHUNK_SIZE);

                    gcount = tempStream.gcount();
                    stream.write(fileBuf, gcount); // only write what was really written so we don't get junk from older files in there
                }

                // not the best code but we depend on failbit
                if (tempStream.fail()
                && gcount == 0)
                    Logging_LogChannel("Error while writing file %s", LogChannel::Warning, realFileName);

                tempStream.close();
            }

            delete[] fileBuf; 
            stream.close();

            Logging_LogChannel("Successfully wrote %d files to beerfile %s!", LogChannel::Message, fileList.size(), path);
            return true;
        }
    };
};

