#include <core/filesystem/filesystem.hpp>

namespace Capy
{
    FilesystemFile* Filesystem::Open(const char* path)
    {
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