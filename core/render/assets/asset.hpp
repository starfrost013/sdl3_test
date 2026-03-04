/*  
    Capybara Wars Project
    A game for Windows & Linux - Copyright © 2025, 2026 starfrost

    asset.hpp: Asset manager and base class
*/

#pragma once
#include <Capy.hpp>

namespace Capy
{
    class Asset
    {
        public: 
            virtual void Load(FilesystemFile* file) { };
    };

    class AssetManager
    {
        public: 
            template <typename T>
            static T* LoadAsset(const char* path)
            {
                FilesystemFile* fs = Filesystem::Open(path);

                if (!fs)
                    return nullptr;
                
                T asset = new T;

                asset->Load();

                return asset; 
            }
    }; 
}
