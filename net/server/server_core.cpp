#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    Cvar* map;

    // TEMPORARY CVARS UNTIL WE HAVE A MENU
    Cvar* mapSizeX;
    Cvar* mapSizeY;

    void Server::Init()
    {
        Logging_LogChannel("Initialising server...", LogChannel::Message);

        socket = NET_CreateDatagramSocket(NULL, port);
        SetMap();

        SetState(ServerState::SERVER_RUNNING);
        Logging_LogChannel("Server initialised!", LogChannel::Message);
    }

    void Server::SetMap()
    {
        // most cvars are created here.
        map = Cvar_Get("map", "", false);
        mapSizeX = Cvar_Get("mapSizeX", "4000", false);
        mapSizeY = Cvar_Get("mapSizeY", "300", false);

        if (strlen(map->string))
        {
            Logging_LogChannel("Setting map to %s...", LogChannel::Message, map->string);
            world.Deserialise(map->string);
        }
        else
        {
            world.SetSize(Vector2(int32_t(mapSizeX->value), int32_t(mapSizeY->value)));
            world.Create();
        }
    }
            
    Server::ServerState Server::GetState()
    {
        return state;
    }

    void Server::SetState(ServerState _state)
    {
        state = _state;
    }

    void Server::ClientStartWorldDownload(Client* client)
    {
        WorldEntity::WorldHeader header = world.GetHeader();
        SendMessage(NetFactory_CreateDownloadStartPacket_Server(map->name, header.size, world.GetSizeInBytes()), client);
    }

    // Ticks the network aft
    void Server::TickNetwork_ConnectedClientMessage(Client* client, NetMsg* msg)
    {
        switch (msg->header.msgType)
        {
            case NetMsgType::NETMSG_WORLD_DOWNLOAD_START:
                ClientStartWorldDownload(client);
                client->connectPhase = Client::CLIENT_DOWNLOADING_WORLD; // server only cares about some of this
                break;
            case NetMsgType::NETMSG_DISCONNECT:
                Logging_LogChannel("Client disconnect from %s", LogChannel::Debug, client->serverOnly.ipStr);
                ClientRemove(client);
                break;
        }
    }

    // Send a message to a client
    void Server::SendMessage(NetMsg msg, Client* client)
    {
        SendMessageToPort(msg, client->serverOnly.address, client->serverOnly.port);
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
                    ClientNew(msg);
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

        if (msg)
            DoneMessage(msg); // delete the message if neede
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
                ClientRemove(client);
        }

        Logging_LogChannel("Shutting down server...", LogChannel::Message);
        NET_DestroyDatagramSocket(socket);
        SetState(ServerState::SERVER_DEAD);
    }
}