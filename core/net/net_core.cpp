#include <core/net/net.hpp>

namespace Capy
{
    void CapyNet_Init()
    {   
        Logging_LogChannel("CapyNet_Init: Initialising network...", LogChannel::Message);

        NET_Init();
    }

    void CapyNet_Shutdown()
    {
        Logging_LogChannel("CapyNet_Shutdown: Shutting down network...", LogChannel::Message);
        NET_Quit();
    }
}

