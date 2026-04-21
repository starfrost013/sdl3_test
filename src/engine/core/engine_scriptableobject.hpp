#include <Capy.hpp>

namespace Capy
{
    #define OBJECT_MAX_NAME_LENGTH         64

    /// @brief Capy Scriptable Object - the base of all scriptable objects
    class CapyScriptableObject
    {
    public: 
    
        virtual void RegisterClass() { };

        //
        // METHODS
        //

        CapyScriptableObject()
        {
            SetName("Generic Scriptable");    
        }

        char* GetName()
        {
            return name;
        }

        void SetName(const char* buf)
        {
            strncpy(this->name, name, OBJECT_MAX_NAME_LENGTH);
        }
    private: 
        // a name 
        char name[OBJECT_MAX_NAME_LENGTH];
    };
}