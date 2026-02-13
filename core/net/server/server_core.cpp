#include <core/net/client/client.hpp>
#include <core/net/server/server.hpp>

namespace Capy
{
    void Server::Init()
    {
        Logging_LogChannel("Initialising server...", LogChannel::Message);

        socket = NET_CreateDatagramSocket(NULL, port);
        SetState(ServerState::SERVER_RUNNING);
    }
            
    Server::ServerState Server::GetState()
    {
        return state;
    }

    void Server::SetState(ServerState _state)
    {
        state = _state;
    }

    // Run while server is in ServerState::UPDATE_RUNNING
    void Server::UpdateWhileRunning()
    {
        NetMessage* msg = GetMessage();

        if (msg 
            && msg->valid)
        {
            Logging_LogChannel("Got some data!!! len=%d", LogChannel::Debug, msg->header.size);
        }
    }

    void Server::Tick()
    {
        switch (state)
        {
            case ServerState::SERVER_RUNNING:
                UpdateWhileRunning();
                break;
            case ServerState::SERVER_SHUTTING_DOWN:
                Shutdown();
                break;
            case ServerState::SERVER_DEAD:
                return;
            
        }
    }

    void Server::Shutdown()
    {
        Logging_LogChannel("Shutting down server...", LogChannel::Message);

        NET_DestroyDatagramSocket(socket);
        
        SetState(ServerState::SERVER_DEAD);
    }
}