#include <core/net/server/server.hpp>

namespace Capy
{
    void Server::Init()
    {
        Logging_LogChannel("Initialising server...", LogChannel::Message);
    }

    void Server::Shutdown()
    {
        Logging_LogChannel("Shutting down server...", LogChannel::Message);
    }
}