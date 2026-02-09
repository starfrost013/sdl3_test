#include <core/net/server/server.hpp>

namespace Capy
{
    void Server::Init()
    {
        Logging_LogChannel("Initialising server...", LogChannel::Message);

        socket = NET_CreateDatagramSocket(NULL, port);
    }
            
    Server::ServerState Server::GetState()
    {
        
    }

    void Server::SetState(ServerState state)
    {
        
    }

    void Server::Shutdown()
    {
        Logging_LogChannel("Shutting down server...", LogChannel::Message);
    }
}