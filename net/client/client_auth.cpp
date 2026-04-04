//
// client_auth.cpp: Client Authentication
//

#include <core/engine.hpp>
#include <core/render/render.hpp>
#include <world/world.hpp>
#include <net/client/client.hpp>

namespace Capy
{
    void Client::Connect(const char* addr)
    {
        // temp
        strncpy(name, playerName->string, CLIENT_NAME_MAX);

        Logging_LogChannel("Client::Connect - Resolving server IP at %s...", LogChannel::Message, addr);
        SetState(ClientState::CLIENT_RESOLVING_ADDRESS);

        serverAddress = NET_ResolveHostname(addr);

        if (!serverAddress)
        {
            Logging_LogChannel("Client::Connect - Failed to resolve hostname!", LogChannel::Error);
            SetState(ClientState::CLIENT_FATAL);
            return; 
        }
    }

    void Client::Disconnect(const char* reason)
    {
        // kicks are the same as disconnects 
        SendMessage(NetFactory_CreateDisconnectPacket(), serverAddress);
        Logging_LogChannel("Disconnected: %s", LogChannel::Debug, reason);
    }

    void Client::ConnectDownloadWorldChunk(NetMsg* msg)
    {
        char winTitleBuf[STRING_MAX] = {0};
        auto totalBytes = world.GetSizeInBytes();

        if (!msg)
            return;

        if (msg->header.type != NETMSG_WORLD_DOWNLOAD_PACKET)
        {
            Logging_LogChannel("ConnectPhase::CLIENT_DOWNLOADING_WORLD: Should be world download packet, but is type %0x :(",
            LogChannel::Warning, msg->header.type);
        }
        else
        {
            memcpy(&world.tileData[downloadProgress], msg->msgData, msg->header.size);
            
            downloadProgress += msg->header.size;
            snprintf(winTitleBuf, STRING_MAX, "Downloading world [%ld/%ld bytes]...", downloadProgress, totalBytes);
            Render_SetWindowTitle(winTitleBuf);
            Logging_LogChannel(winTitleBuf, LogChannel::Debug);

            if (downloadProgress >= totalBytes)
            {
                // the server will be done sending by now. anything else is ignored, but we send an ACK to the server
                Render_SetWindowTitle("Downloaded world!");

                connectPhase = ConnectPhase::CLIENT_LETS_GO;
                return;
            }
        }
    }

    // Update network before the connection has been fully completed.
    void Client::TickNetworkConnecting(NetMsg* msg)
    {
        switch (connectPhase)
        {
            case ConnectPhase::CLIENT_HELLO:
                Logging_LogChannel("Client sending hello", LogChannel::Debug);
                SendMessage(NetFactory_CreateClientHelloPacket(name), serverAddress);
                connectPhase = ConnectPhase::CLIENT_HELLO_SENT;
                break;
            // wait to see if we were allowed in
            case ConnectPhase::CLIENT_HELLO_SENT: // TODO: figure out a better way of doing this than extra states that prevents it being sent multiple times
                if (msg)
                {
                    if (msg->header.type != NetMsgType::NETMSG_SERVER_HELLO)
                    {
                        Logging_LogChannel("Client::TickNetworkConnecting - Server sent back non-server hello!", LogChannel::Warning);
                        return;
                    }

                    Logging_LogChannel("Client received hello", LogChannel::Debug);

                    // change state

                    NetHelloStatus result = static_cast<NetHelloStatus>(msg->Read<uint8_t>());
                    bool success = (result == NetHelloStatus::HELLO_OK);

                    if (success)
                    {
                        Logging_LogChannel("Client hello accepted", LogChannel::Debug);
                        connectPhase = ConnectPhase::CLIENT_DOWNLOAD_WORLD;
                    }
                    else
                    {
                        const char* errMsg = "TODO: Duplicated username handling\n";

                        if (result == NetHelloStatus::HELLO_DUPLICATE_CLIENT)
                            errMsg = "This device is already connected!";
                        else if (result == NetHelloStatus::HELLO_GO_AWAY)
                            errMsg = "Piss Off";
                        else if (result == NetHelloStatus::HELLO_INVALID_VERSION)
                            errMsg = "Incorrect client protocol version (try updating the game)";
                        else if (result == NetHelloStatus::HELLO_TOO_MANY)
                            errMsg = "Server is full!";

                        Logging_LogChannel("Connection rejected: %s", LogChannel::Error, errMsg);
                        SetState(ClientState::CLIENT_UNCONNECTED);
                    }
                }
                break; 
            // ask for map header
            case ConnectPhase::CLIENT_DOWNLOAD_WORLD:
                Logging_LogChannel("Initiating world download...", LogChannel::Debug);
                SendMessage(NetFactory_CreateDownloadStartPacket_Client(), serverAddress);
                connectPhase = ConnectPhase::CLIENT_DOWNLOAD_WORLD_SENT;
                break;
            // wait for map header
            case ConnectPhase::CLIENT_DOWNLOAD_WORLD_SENT:
                if (msg)
                {
                    char* mapName = msg->Read<char*>();
                    Vector2<int32_t> mapSize = msg->Read<Vector2<int32_t>>();
                    uint32_t expectedMapBytes = msg->Read<uint32_t>();

                    Logging_LogChannel("Starting to download map %s %dx%d, %d bytes", LogChannel::Debug,
                    mapName, mapSize.x, mapSize.y, expectedMapBytes);

                    world.Init(mapSize);
                    connectPhase = ConnectPhase::CLIENT_DOWNLOADING_WORLD;
                }

                break;
            // client is downloading the world
            case ConnectPhase::CLIENT_DOWNLOADING_WORLD:
                ConnectDownloadWorldChunk(msg);
                break;
            // we are done
            case ConnectPhase::CLIENT_LETS_GO:     // we are done
                SetState(ClientState::CLIENT_CONNECTED);
                break;
        }

    }
}