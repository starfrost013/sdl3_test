#pragma once
#include <Capy.hpp>

class NetMsg;
class Entity; // needed for netIdentities

/* entity.hpp: Core Entity code */

namespace Capy
{    
    
    class Entity 
    { 
        private: 
            static std::unordered_map<const char*, Entity* (*)()> netIdentities;

        public: 
            static const char* classname; 
 
            // 
            // Events
            //
            virtual void Create() { };                        // Called on object creation.
            virtual void Tick() { };                            // Called every tick.
            virtual void Render() { };                         // Called every frame.
            virtual void Destroy() { };                        // Called on object destruction

            static void CreateRequest(NetMsg* msg) { };
            virtual void ReadRequest(NetMsg* msg) { };
            virtual void UpdateRequest(NetMsg* msg) { };
            virtual void DeleteRequest(NetMsg* msg) { };

            // Checks if an entity has a network identity.
            static bool HasNetIdentity(const char* classname)
            {
                if (!classname)
                {
                    Logging_LogChannel("Entity_HasNetIdentity: invalid classname", LogChannel::Warning);
                    return false; 
                }

                if (!netIdentities[classname])
                {
                    Logging_LogChannel("Entity_HasNetIdentity: class %s does not exist!", LogChannel::Warning, classname);
                    return false; 
                }

                return true;
            }
    };

}

