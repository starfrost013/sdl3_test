#include <Capy.hpp>

namespace Capy
{
    /// @brief Capy Scriptable Object - the base of all scriptable objects
    class CapyScriptableObject
    {
        virtual void RegisterClass() { };
    };
}