#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    void Server::Init()
    {
        Logging_LogChannel("Initialising server...", LogChannel::Message);

        socket = NET_CreateDatagramSocket(NULL, port);
        SetState(ServerState::SERVER_RUNNING);
        Logging_LogChannel("Server initialised!", LogChannel::Message);

    }
            
    Server::ServerState Server::GetState()
    {
        return state;
    }

    void Server::SetState(ServerState _state)
    {
        state = _state;
    }

    // Ticks the network aft
    void Server::TickNetwork_ConnectedClientMessage(Client* client, NetMsg* msg)
    {
        switch (msg->header.msgType)
        {
            case NetMsgType::NETMSG_WORLD_DOWNLOAD_START:
                break;
            case NetMsgType::NETMSG_DISCONNECT:
                Logging_LogChannel("Client disconnect from %s", LogChannel::Debug, client->serverOnly.ipStr);
                RemoveClient(client);
                break;
        }
    }

    // Run while server is in ServerState::UPDATE_RUNNING
    void Server::TickNetwork()
    {
        NetMsg* msg = GetMessage();

        if (msg 
            && msg->valid)
        {
            switch (msg->header.msgType)
            {
                // Messages that don't require a client
                case NetMsgType::NETMSG_HELLO:
                    NewClient(msg);
                    break; 
                default:
                    Client* client = GetMessageSender(msg->addr);

                    if (!client)
                    {
                        Logging_LogChannel("Packet from non-client IP %s, ignoring...", LogChannel::Warning, NET_GetAddressString(msg->addr));
                        return;
                    }
                    
                    // not fully connected until character spawn request is fulfilled
                    TickNetwork_ConnectedClientMessage(client, msg);

                    break;
            }

        }
    }

    // Utility method that gets the client that sent a certain message frrom its address
    Client* Server::GetMessageSender(NET_Address* address)
    {
        for (Client* client : clients)
        {
            if (client != nullptr)
            {
                if (!NET_CompareAddresses(client->serverOnly.address, address))
                    return client;
            }
        }

        return nullptr;
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