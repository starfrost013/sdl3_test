#include <Capy.hpp>

namespace Capy
{
    template <typename T>
    class Component : CapyScriptableObject
    {
        virtual void RegisterClass() override;
    };
}