 #include <core/net/client/client.hpp>

namespace Capy
{
    void Client::Init()
    {
        Logging_LogChannel("Initialising client...", LogChannel::Message);

    }

    void Client::Shutdown()
    {
        Logging_LogChannel("Shutting down client...", LogChannel::Message);
    }
}