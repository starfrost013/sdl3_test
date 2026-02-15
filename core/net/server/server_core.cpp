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
    void Server::TickNetwork()
    {
        NetMessage* msg = GetMessage();

        if (msg 
            && msg->valid)
        {
            switch (msg->header.msgType)
            {
                case NetMessageType::NETMSG_HELLO:
                    NewClient(msg);

                    break; 
            }

        }
    }

    void Server::Tick()
    {
        switch (state)
        {
            case ServerState::SERVER_RUNNING:
                TickNetwork();
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
        for (auto client : clients)
        {
            // todo: send "server is shutting down..." message
            
            if (client != nullptr)
                RemoveClient(client);
        }

        Logging_LogChannel("Shutting down server...", LogChannel::Message);
        NET_DestroyDatagramSocket(socket);
        SetState(ServerState::SERVER_DEAD);
    }
}