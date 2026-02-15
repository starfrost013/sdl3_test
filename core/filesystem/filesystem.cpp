#include <core/filesystem/filesystem.hpp>

namespace Capy
{
    Cvar* baseDirectory; 

    FilesystemSettings Filesystem::settings;

    /* Safe to be a global because it is only used once at initialisation time */
    FilesystemSettings defaultFilesystemSettings =
    {
        .useBaseDirectory = true,
        .baseDirectory = "assets",
    };

    void Filesystem::Init(FilesystemSettings newSettings)
    {

        Filesystem::settings = newSettings;


    }

    FilesystemFile* Filesystem::Open(const char* path)
    {
        if (settings.useBaseDirectory)
        {
            char path[STRING_MAX_GENERIC] = {0};

            char* lastPartOfPath = strrchr(path, std::filesystem::path::preferred_separator);

            // get rid of the directory path if we don't need it
            if (lastPartOfPath)
                strncpy(path, lastPartOfPath, strlen(lastPartOfPath));

            snprintf(path, STRING_MAX_GENERIC, "%s%s", settings.baseDirectory, lastPartOfPath);
        }

        FilesystemFile* ff = new FilesystemFile;

        ff->file.open(path, std::ios_base::in | std::ios_base::out);
        ff->open = true;

        return ff;
    }

    void Filesystem::Close(FilesystemFile* ff)
    {
        ff->file.close();
        
        delete ff; 
    }
};