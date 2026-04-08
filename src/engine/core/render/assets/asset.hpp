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
            virtual void Load(const char* path) { };
            virtual void Unload() { };
            
            // not all assets need to be backed by files, but some can be (or custom loaders e.g. STBi can be used)
            virtual void LoadFile(FilesystemFile* file) { };
    };

    class AssetManager
    {
        public: 
            // Load an asset not backed by a file.
            template <typename T>
            static T* Load(const char* path)
            { 
                T asset = new T;
                asset->Load(path);
                return asset; 
            }

            // Load an asset backed by a file. (bit janky)
            template <typename T>
            static T* LoadFile(const char* path)
            {
                FilesystemFile* fs = Filesystem::Open(path);

                if (!fs)
                    return nullptr; 

                T asset = new T;
                asset->LoadFile(fs);
                return asset; 
            }
    }; 
}

