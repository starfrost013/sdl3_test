//
// Texture system
// Implemented as an asset type
//

#pragma once

#include <Capy.hpp>
#include <core/render/assets/asset.hpp>

// stbi stuff we need 
typedef Uint8 stbi_uc;

namespace Capy
{
    class AssetTexture : public Asset
    {
        public: 
            void Load(const char* path) override; 
            void LoadFile(FilesystemFile* file) override; 

            void Unload() override; 

            void SetSize(Vector2<int32_t> _size)
            {
                size = _size; 
            }
        private: 
            Vector2<int32_t> size;    
            int32_t bpp;

            stbi_uc* data = nullptr; 
    };
}
