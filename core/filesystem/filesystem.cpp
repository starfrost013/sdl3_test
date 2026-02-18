#include <core/cvar/cvar.hpp>
#include <core/filesystem/filesystem.hpp>

namespace Capy
{
    Cvar* fsBasedir; 

    void Filesystem::Init()
    {
        fsBasedir = Cvar_Get("fsBasedir", "./assets", false);
        Logging_LogChannel("fsBasedir is %s", LogChannel::Debug, fsBasedir->string);
    }

    FilesystemFile* Filesystem::Open(const char* path)
    {
        char finalPathBuf[STRING_MAX_GENERIC] = {0};

        // lol this is such a screwed up language
        char* lastPartOfPath = strrchr((char*)finalPathBuf, std::filesystem::path::preferred_separator);

        // get rid of the directory path if we don't need it
        if (lastPartOfPath)
            strncpy(finalPathBuf, lastPartOfPath, strlen(lastPartOfPath));

        snprintf(finalPathBuf, STRING_MAX_GENERIC, "%s%s", fsBasedir->string, lastPartOfPath);

        FilesystemFile* ff = new FilesystemFile;

        // just trunc the file
        // we already have the version from deserialisation
        ff->stream.open(finalPathBuf, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        ff->open = true;

        return ff;
    }

    void Filesystem::Close(FilesystemFile* ff)
    {
        ff->stream.close();
        
        delete ff; 
        ff = nullptr;
    }
};