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
    FilesystemImage* imageListHead;
    FilesystemImage* imageListTail;
    
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

    /// @brief Internal open method. Allows files to be opened from a pak file or filesystem.
    /// @param path The path to the file to open.
    /// @param mode The mode of the file.
    /// @param type The filesystem type (FS or PACK FILE) to use for opening this file.
    /// @return A pointer to a FilesystemFile object.
    FilesystemFile* Filesystem::OpenInternal(const char* path, FilesystemFileMode mode, FilesystemFileType type)
    {
        switch (type)
        {
            case FilesystemFileType::FILETYPE_FS:
                char finalPathBuf[STRING_MAX] = {0};

                snprintf(finalPathBuf, STRING_MAX, "%s%s", fsBasedir->string, path);

                FilesystemFile* ff = new FilesystemFile;
                
                // just trunc the file
                // we already have the version from deserialisation

                auto iosMode = std::ios_base::in | std::ios_base::out | std::ios_base::trunc;

                if (mode & FILE_BINARY)
                    iosMode |= std::ios_base::binary;
                
                ff->stream->open(finalPathBuf, iosMode);

                if (ff->stream->bad())
                {
                    delete ff; 
                    return nullptr;
                }

                strncpy(ff->path, finalPathBuf, MAX_PATH);
                ff->open = true;
                return ff;
            case FilesystemFileType::FILETYPE_PAK:
                char buf[MAX_PATH] = {0}; //allocate a temporary buffer to find the image

                const char* firstColon = strchr(path, FILESYSTEM_PACKAGE_SEPARATOR);

                if (!firstColon)
                {
                    Logging_LogChannel("Filesystem::OpenInternal - somehow no beer in FilesystemFileType::FILETYPE_PAK",
                    LogChannel::Error);
                    return nullptr;
                }

                // lop off the "beer!" thing
                const char* initial = path + strlen(FILESYSTEM_PACKAGE_NAMESPACE);
                size_t bufLength = firstColon - initial;
                const char* filenameWithinPak = path + bufLength;

                strncpy(buf, initial, bufLength);
                FilesystemImage* image = FS_GetImageByName(path);

                if (!image)
                {
                    Logging_LogChannel("Filesystem::OpenInternal - couldn't find image with path %s (FilesystemFileType = FilesystemFileType::FILETYPE_PAK)",
                    LogChannel::Error);
                    return nullptr;
                }

                // copy the rest of the string back in
                strncpy(buf, filenameWithinPak, MAX_PATH);
                FilesystemFile* ff = image->OpenFile(buf);

                return ff; 
        }

        return nullptr; 
    }

    FilesystemFile* Filesystem::Open(const char* path, FilesystemFileMode mode)
    {
        // check for beer file path - ensure it is long enough and contains a colon
        if (strstr(path, FILESYSTEM_PACKAGE_NAMESPACE)
        && strchr(path, FILESYSTEM_PACKAGE_SEPARATOR))
        {
            return OpenInternal(path, mode, FilesystemFileType::FILETYPE_PAK);
        }
        else
            return OpenInternal(path, mode, FilesystemFileType::FILETYPE_FS);
    }

    void Filesystem::Close(FilesystemFile* ff)
    {
        // could be called into while null if file was not found or did not open
        if (!ff)
            return;

        ff->stream->close();
        
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

    bool FilesystemImage::Open(const char* path)
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

        stream->open(path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

        if (stream->bad())
        {
            Logging_LogChannel("Failed to open beerfile: Could not open file", LogChannel::Error);
            return false;
        }

        strncpy(this->path, path, MAX_PATH);

        return true; 
    }

    bool FilesystemImage::Read(const char* path)
    {
        if (!Open(path))
            return false;

        stream->read((char*)(&header), sizeof(FilesystemImageHeader));
        
        if (header.magic != FILESYSTEM_PACKAGE_MAGIC)
        {
            Logging_LogChannel("Failed to read beerfile: Invalid header: Magic is not %4c", LogChannel::Error, header.magic);
            return false;
        }

        if (header.version != FILESYSTEM_PACKAGE_MAGIC)
        {
            Logging_LogChannel("Failed to read beerfile: File is version %d, %d expected", LogChannel::Error, header.version, FILESYSTEM_PACKAGE_VERSION);
            return false;
        }

        if (!header.numFiles)
        {
            Logging_LogChannel("This beerfile has no files. No point reading it.", LogChannel::Warning, header.version, FILESYSTEM_PACKAGE_VERSION);
            return false;
        }

        // seems slow, must be a faster way to do this, can improve later. is it really worth saving the space ?
        // problem: long paths ( > 255 chars). COuld write a 16-bit...hmmm...would use more space...
        int32_t len = 0;
        char curByte = 0;

        /* Read the file headers */
        for (auto i = 0; i < header.numFiles; i++)
        {
            FilesystemImageFileEntry fileEntry = {0};

            stream->read((char*)(&fileEntry.size), sizeof(size_t));          
            stream->read((char*)(&fileEntry.location), sizeof(size_t));

            while (len < MAX_PATH && curByte != '\0')
            {
                curByte = stream->get();
                len++;
            }

            fileEntry.id = i;

            // go back
            stream->seekg(-len, std::ios_base::cur);
            stream->read(fileEntry.path, len);  
        }

        // add this image to the linked list of images (Capy only)
        // can't unload them
        if (!imageListHead)
            imageListHead = imageListTail = this;
        else 
        {
            imageListTail->next = this;
            imageListTail = this;
        }
    }

    bool FilesystemImage::Write(const char* path)
    {
        if (!Open(path))
            return false; 

        //ensure the magic is there
        header.magic = FILESYSTEM_PACKAGE_MAGIC;
        header.version = FILESYSTEM_PACKAGE_VERSION;

        stream->write((char*)(&header), sizeof(header));

        size_t currentLocation = 0;

        // write the image header
        for (FilesystemImageFileEntry entry : fileList)
        {
            // only write the actual length of things
            stream->write((char*)(&entry.size), sizeof(size_t));
            // now write the location
            entry.location = currentLocation;
            currentLocation += entry.size;
            stream->write((char*)(&entry.location), sizeof(size_t));
            stream->write((char*)(&entry.path), strlen(entry.path) + 1); // include the null terminator. should we change this
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
                stream->write(fileBuf, gcount); // only write what was really written so we don't get junk from older files in there
            }

            // not the best code but we depend on failbit
            if (tempStream.fail()
            && gcount == 0)
                Logging_LogChannel("Error while writing file %s", LogChannel::Warning, realFileName);

            tempStream.close();
        }

        delete[] fileBuf; 
        stream->close();

        Logging_LogChannel("Successfully wrote %d files to beerfile %s!", LogChannel::Message, fileList.size(), path);
        return true;
    }

    /// @brief Get a file entry for a path within the current filesystem image.
    /// @param name the name of the file to obtain
    /// @return the FilesystemImageFileEntry for the name name, or nullptr if not found
    FilesystemImageFileEntry* FilesystemImage::GetFileByPath(const char* name)
    {
        for (FilesystemImageFileEntry fileEntry : fileList)
        {
            if (!strncmp(fileEntry.path, name, MAX_PATH))
                return &fileEntry;
        }

        return nullptr; 
    }

    FilesystemFile* FilesystemImage::OpenFile(const char* name)
    {
        FilesystemImageFileEntry* imageEntry = GetFileByPath(name);

        if (!imageEntry)
        {
            Logging_LogChannel("FilesystemImage::OpenFile - could not find path %s in pakfile %s",
            LogChannel::Error, name, path);    // Some helper functions
        }

        size_t currentLocation = 0;

        for (int32_t i = 0; i < imageEntry->id; i++)
        {
            if (i > fileList.size()) // wat
            {
                Logging_LogChannel("Not really an assert: FilesystemImage::OpenFile: Bing Bong Big Bug", LogChannel::Fatal);
                break; 
            }

            currentLocation += fileList[i].size;
        }

        // calculate the 
        stream->seekg(currentLocation);

        FilesystemFile* file = new FilesystemFile;

        file->open = true;
        // of the format "beer!pak0.pak:hhehehe" so that we only have to look for one character
        snprintf(file->path, MAX_PATH, "%s%s:%s", FILESYSTEM_PACKAGE_NAMESPACE, this->path, name);
        file->stream = stream;

        return file; 
    }

    void FilesystemImage::Close()
    {
        stream->close();
    }

    // Some helper functions
    FilesystemImage* FS_GetImageByName(const char* name)
    {
        FilesystemImage* image = imageListHead;

        while (image)
        {
            if (!strcmp(image->path, name))
                return image;

            image = image->next;
        }
        
        return nullptr;
    }
};