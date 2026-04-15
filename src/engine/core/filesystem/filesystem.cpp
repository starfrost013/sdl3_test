#include <core/cvar/cvar.hpp>
#include <core/filesystem/filesystem.hpp>
using FilesystemImageFileEntry = Capy::FilesystemImage::FilesystemImageFileEntry;

namespace Capy
{
    //
    // GLOBALS
    //

    Cvar* fsBasedir; 
    // first member of filesystem image chain
    FilesystemImage* firstImage;
    
    //
    // METHODS
    //

    void Filesystem::Init()
    {
        fsBasedir = Cvar_Get("fsBasedir", "./", false);

        Logging_LogChannel("fsBasedir is %s", LogChannel::Debug, fsBasedir->string);

        if (!std::filesystem::exists(fsBasedir->string))
        {
            Logging_LogChannel("Creating the basedir, as it doesn't exist! (You probably won't have many assets...)", LogChannel::Debug);
            std::filesystem::create_directory(fsBasedir->string);
        }

    }

    void Filesystem::OpenImage(const char* path)
    {
        Logging_LogChannel("Mounting beer image... %s", LogChannel::Message, path); 

    }

    FilesystemFile* Filesystem::Open(const char* path, FilesystemFileMode mode)
    {
        // get beered
        // worst ever code
        if (strstr(path, FILESYSTEM_PACKAGE_EXTENSION))
        {
            OpenImage(path);
            return nullptr;  // don't use the return value
        }

        char finalPathBuf[STRING_MAX] = {0};

        snprintf(finalPathBuf, STRING_MAX, "%s%s", fsBasedir->string, path);

        FilesystemFile* ff = new FilesystemFile;
        
        // just trunc the file
        // we already have the version from deserialisation

        auto iosMode = std::ios_base::in | std::ios_base::out | std::ios_base::trunc;

        if (mode & FILE_BINARY)
            iosMode |= std::ios_base::binary;
        
        ff->stream.open(finalPathBuf, iosMode);

        if (ff->stream.bad())
        {
            delete ff; 
            return nullptr;
        }

        strncpy(ff->path, finalPathBuf, MAX_PATH);

        ff->open = true;

        return ff;
    }

    void Filesystem::Close(FilesystemFile* ff)
    {
        // could be called into while null if file was not found or did not open
        if (!ff)
            return;

        ff->stream.close();
        
        delete ff; 
        ff = nullptr;
    }

    //
    // IMAGE
    //

    
    bool FilesystemImage::AddFile(const char* path)
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
        size_t basedirLength = strlen(fsBasedir->string);

        size_t endOfBasedirPos = 0;

        for (endOfBasedirPos = 0; endOfBasedirPos < entryLength; endOfBasedirPos++)
        {
            // don't start comparing garbage
            if (endOfBasedirPos >= basedirLength)
                break;

            if (entry.path[endOfBasedirPos] != fsBasedir->string[endOfBasedirPos])
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

    bool FilesystemImage::Write(const char* path)
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
            snprintf(realFileName, MAX_PATH * 2, "%s%c%s", fsBasedir->string, std::filesystem::path::preferred_separator, entry.path);

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