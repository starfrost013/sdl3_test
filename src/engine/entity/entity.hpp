#pragma once
#include <Capy.hpp>

class NetMsg;
class Entity; // needed for netIdentities

/* entity.hpp: Core Entity code */

namespace Capy
{   
    // Based on observation of unity, this seems to be the largest reasonable number. 
    #define INITIAL_COMPONENT_LIST_SIZE         8 

    class Entity : CapyScriptableObject
    { 
        private: 
            static std::unordered_map<const char*, Entity* (*)()> netIdentities;

        public: 
            static const char* classname; 
            std::vector<Component*> components;                 // The components attached to this Entity.

            // 
            // Events
            //
            virtual void Create() { };                          // Called on object creation.
            virtual void Tick() { };                            // Called every tick.
            virtual void Render() { };                          // Called every frame.
            virtual void Destroy()                              // Called on object destruction
            {
                // delete all the components
                for (Component* component : components)
                {
                    component->Destroy();
                    delete component;
                }
            }

            virtual void CreateRequest(NetMsg* msg) { };         // Called on create RPC
            virtual void ReadRequest(NetMsg* msg) { };          // Called on read RPC
            virtual void UpdateRequest(NetMsg* msg) { };        // Called on update RPC
            virtual void DeleteRequest(NetMsg* msg) { };        // Called on delete RPC

            Entity()
            {
                components.reserve(INITIAL_COMPONENT_LIST_SIZE);
            }

            template <std::derived_from<Component> T>
            Component* GetComponent()
            {
                for (Component* component : components)
                {
                    if (std::is_same(decltype(component), T))
                        return component; 
                }
            }

            template <std::derived_from<Component> T>
            Component* AddComponent()
            {
                T component = new T();

                components.push_back(&component);
            }

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

