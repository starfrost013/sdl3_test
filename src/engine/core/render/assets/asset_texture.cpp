//
// Texture system implementation
// Implemented as an asset type
//

#include <Capy.hpp>
#include <core/render/assets/asset.hpp>
#include <core/render/assets/asset_texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <external/stbi/stb_image.h>

namespace Capy
{
    void AssetTexture::Load(const char* path)
    {
        int32_t comp = 0; 

        data = stbi_load(path, &size.x, &size.y, &comp, 0);
        bpp = comp << 3;
   
        if (!data)
        {
            Logging_LogChannel("AssetTexture::Load - failed to load %s", LogChannel::Error, path);
            return; 
        }

        // copy the image data
        auto imageSize = size.x * size.y * comp;
    }

    // this is a bit silly
    void AssetTexture::LoadFile(FilesystemFile* file)
    {
        // We could read in all the data, but that seems to be pretty dumb. SO let's just do this for now
        Logging_LogChannel("JANK WARNING: AssetTexture::LoadFile is not really recommended for now (Change this call to AssetTexture::Load)", LogChannel::Warning);

        // load using stbi_load. Figure out how to, like, make this less crap
        char newPath[MAX_PATH];
    
        strncpy(newPath, file->path, MAX_PATH);
        Filesystem::Close(file);
        Load(newPath);
    }

    void AssetTexture::Unload()
    {
        if (!data)
            return;

        stbi_image_free(data);
        data = nullptr; 
    }
}