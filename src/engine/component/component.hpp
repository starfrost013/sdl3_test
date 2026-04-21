#include <Capy.hpp>

namespace Capy
{
    /// @brief AN 
    /// @tparam THe type of the component...
    class Component : public CapyScriptableObject
    {
    public:
        virtual void Start() { };                       // Called on Entity initialisation
        virtual void Frame() { };                       // Called on Frame
        virtual void Tick() { };                        // Called on Tick 
        virtual void Destroy() { };                     // Called on Destroy
    };
}