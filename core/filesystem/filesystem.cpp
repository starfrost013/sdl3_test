#include <core/cvar/cvar.hpp>
#include <core/filesystem/filesystem.hpp>

namespace Capy
{
    Cvar* fsBasedir; 

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
        if (!strstr(path, FILESYSTEM_PACKAGE_EXTENSION))
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
};