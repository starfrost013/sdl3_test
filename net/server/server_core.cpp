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

        if (IsDedicated())
        {
            // non-blocking console input
            consoleInputProc = std::async(std::launch::async, &Server::ConsoleInputThread);
        }

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
            world.Init(Vector2(int32_t(mapSizeX->value), int32_t(mapSizeY->value)));
            world.Generate();
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
        World::WorldHeader header = world.GetHeader();
        SendMessage(NetFactory_CreateDownloadStartPacket_Server(map->name, header.size, world.GetSizeInBytes()), client);

        client->connectPhase = Client::ConnectPhase::CLIENT_DOWNLOADING_WORLD;
    }

    // Fill the client's buffer with world data every chunk. (Should only need a slight refactor for generic downloads, but we probably won't need those.)
    void Server::ClientSendWorldChunk(Client* client)
    {
        auto worldSize = world.GetSizeInBytes();

        // don't send too many at once
        //for (auto i = 0; i < 1; i++) //TODO: FIX
        //{
        auto size = 0;
        size_t remainingBytes = (worldSize - client->downloadProgress);

        if (remainingBytes < MAX_PACKET_SIZE)
            size = remainingBytes;
        else    
            size = MAX_PACKET_SIZE;

        NetMsg packetMsg = NetFactory_CreateDownloadPacket(size);

        // we don't care about size checks since we know the size, just smash it in
        packetMsg.header.size = size;
        uint8_t* tileData = world.GetWorldTileData();

        memcpy(packetMsg.msgData, (void*)&tileData[client->downloadProgress], size);
        SendMessage(packetMsg, client); 

        client->downloadProgress += size;

        if (client->downloadProgress >= worldSize)
        {
            Logging_LogChannel("Download is done!", LogChannel::Debug);
            client->connectPhase = Client::ConnectPhase::CLIENT_LETS_GO; //value does not really matter on server end. no other special stuff needed
            return;
        }
        //}
    }

    // Ticks the network when the server receives a message from a client
    void Server::TickNetwork_ClientMessage(Client* client, NetMsg* msg)
    {  
        switch (msg->header.msgType)
        {
            case NetMsgType::NETMSG_WORLD_DOWNLOAD_START:
                ClientStartWorldDownload(client);
                Logging_LogChannel("Downloading world to client (not cached)", LogChannel::Message);
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

    // Send a message to all clients
    void Server::SendMessageToAll(NetMsg msg)
    {
        SendMessageToAll(msg, nullptr);
    }

    // Send a message to all clients, except that indicated with the "exclude"
    void Server::SendMessageToAll(NetMsg msg, Client* exclude)
    {
        for (Client* client : clients)
        {
            if (client && 
                client != exclude)
            {
                SendMessage(msg, client);
            }
        }
    }

    // Get a client by its username.
    Client* Server::ClientByUsername(const char* username)
    {
        for (Client* client : clients)
        {
            if (!client)
                continue; 

            if (!strncmp(client->name, username, CLIENT_NAME_MAX))
                return client; 
        }

        return nullptr; 
    }

    // is this a good idea?
    void Server::TickDownloads()
    {
        for (Client* client : clients)
        {
            if (client
            && client->connectPhase == Client::ConnectPhase::CLIENT_DOWNLOADING_WORLD)
            {
                ClientSendWorldChunk(client);
            }
        }
    }

    // Run while server is in ServerState::UPDATE_RUNNING
    void Server::TickNetwork()
    {
        // first tick downloads
        TickDownloads();

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
                    Client* client = ClientByIp(msg);

                    if (!client)
                    {
                        Logging_LogChannel("Packet from non-client IP %s, ignoring...", LogChannel::Warning, NET_GetAddressString(msg->addr));
                        return;
                    }
                    
                    // not fully connected until character spawn request is fulfilled
                    TickNetwork_ClientMessage(client, msg);

                    break;
            }

        }

        if (msg)
            DoneMessage(msg); // delete the message if neede
    }

    // Utility method that gets the client that sent a certain message frrom its address
    Client* Server::ClientByIp(NetMsg* msg)
    {
        for (Client* client : clients)
        {
            if (client != nullptr)
            {
                if (!NET_CompareAddresses(client->serverOnly.address, msg->addr))
                    return client;
            }
        }

        return nullptr;
    }

    Client* Server::ClientByIpPort(NetMsg* msg)
    {
        Client* client = ClientByIp(msg);

        return (client->serverOnly.port == msg->port) ? client : nullptr;
    }

    // Main server tick function (runs at a set tickrate)
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

    // Main server frame function (runs AFAP)
    void Server::Frame()
    {
        NetMode::Frame();

        // get the latest console lines and shove them into the command system
        if (IsDedicated())
            ConsoleUpdate();
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