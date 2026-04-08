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
            virtual void Create() { };                          // Called on object creation.
            virtual void Tick() { };                            // Called every tick.
            virtual void Render() { };                          // Called every frame.
            virtual void Destroy() { };                         // Called on object destruction

            static void CreateRequest(NetMsg* msg) { };         // Called on create RPC
            virtual void ReadRequest(NetMsg* msg) { };          // Called on read RPC
            virtual void UpdateRequest(NetMsg* msg) { };        // Called on update RPC
            virtual void DeleteRequest(NetMsg* msg) { };        // Called on delete RPC

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

