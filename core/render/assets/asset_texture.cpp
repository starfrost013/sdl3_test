//
// Texture system implementation
// Implemented as an asset type
//

#include <Capy.hpp>
#include <core/render/assets/asset.hpp>
#include <core/render/assets/asset_texture.hpp>

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

    void AssetTexture::Unload()
    {
        if (!data)
            return;

        stbi_image_free(data);
        data = nullptr; 
    }
}