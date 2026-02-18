// entity_world.cpp: World load/save code

#include <data/entities/entity_world.hpp>
#include <core/game.hpp>
#include <data/entities/entity.hpp>
#include <util/util.hpp>


namespace Capy
{
    // Open the world file (returns true if it succeeded or is already open, returns false if it failed)
    bool WorldEntity::OpenWorldFile(const char* fileName = WORLD_DEFAULT_FILENAME)
    {
        // open the file if it does not exist
        if (!file)
        {
            file = Filesystem::Open(fileName);
            Logging_LogChannel("Failed to open world file at %s!", LogChannel::Error, fileName);
            return false; 
        }

        return true; 
    }

    void WorldEntity::CloseWorldFile()
    {
        Filesystem::Close(file);
    }

    // Save level
    bool WorldEntity::Serialise(const char* fileName = WORLD_DEFAULT_FILENAME)
    {        
        Logging_LogChannel("Serialising level at %s", LogChannel::Debug, fileName);

        if (!OpenWorldFile())
            return false;

        // set a default world name
        if (!header.name)   
            strncpy(header.name, fileName, WORLD_NAME_LENGTH);

        file->stream << header.name;
        file->stream << header.size;
        file->stream << WORLD_FILE_FORMAT_VERSION;
        
        auto finalSize = header.size.x * header.size.y; // really need to create a property that automates ths

        file->stream.write((char*)world, finalSize);
        
        return true; 
    }

    // Called during level loading
    bool WorldEntity::Deserialise(const char* fileName = WORLD_DEFAULT_FILENAME)
    {        
        Logging_LogChannel("Deserialising level at %s", LogChannel::Debug, fileName);

        bool success = false; 
        auto size = 0;

        // try and open the world file
        if (!OpenWorldFile())
            goto done;

        file->stream >> header.name;
        file->stream >> header.size;
        file->stream >> header.version;

        size = header.size.x * header.size.y;

        if (header.version != WORLD_FILE_FORMAT_VERSION)
        {
            Logging_LogChannel("Failed to open world: This world is not compatible with this version of " APP_NAME ". Version was %d, expected %d",
            LogChannel::Error, header.version, WORLD_FILE_FORMAT_VERSION);
            goto done;  
        }

        if (header.size.x <= 0
        || header.size.y <= 0)
        {  
            Logging_LogChannel("This world has an invalid size!", LogChannel::Error);
            goto done; 
        }

        world = new uint8_t[size];

        Logging_LogChannel("About to load world data for world:\n"
            "Name = %s\n"
            "Size in bytes = %d\n"
            "Size in tiles = %d x %d\n"
            "Format version = %d", LogChannel::Debug,
            header.name, size, header.size.x, header.size.y, header.version 
        );

        // read in the world
        file->stream.read((char*)world, size);

        if (file->stream.bad())
        {
            Logging_LogChannel("The data of this world is truncated!", LogChannel::Error);
            goto done; 
        }

        success = true; // set success 

    done:
        CloseWorldFile();
        return success; 
    }
}