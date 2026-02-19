//
// client_auth.cpp: Client Authentication
//

#include <core/game.hpp>
#include <core/render/render.hpp>
#include <data/entities/entity_world.hpp>
#include <net/client/client.hpp>

namespace Capy
{
    void Client::Connect(const char* addr)
    {
        Logging_LogChannel("Client::Connect - Trying to connect to server at %s...", LogChannel::Message, addr);
        SetState(ClientState::CLIENT_RESOLVING_ADDRESS);

        serverAddress = NET_ResolveHostname(addr);

        if (!serverAddress)
        {
            Logging_LogChannel("Client::Connect - Failed to resolve hostname!", LogChannel::Error);
            SetState(ClientState::CLIENT_FATAL);
            return; 
        }
    }

    void Client::Disconnect()
    {
        SendMessage(NetFactory_CreateDisconnectPacket(), serverAddress);
    }

    void Client::ConnectOnResolveDone(NetMsg* msg)
    {
        /* temp */
        //world.SetSize(Vector2(3000, 400));
        //world.Create();
        
        switch (connectPhase)
        {
            case ClientConnectionPhase::CLIENT_HELLO:
                Logging_LogChannel("Client sending hello", LogChannel::Debug);
                SendMessage(NetFactory_CreateClientHelloPacket(), serverAddress);
                connectPhase = ClientConnectionPhase::CLIENT_HELLO_SENT;
                break;
            case ClientConnectionPhase::CLIENT_HELLO_SENT: // TODO: figure out a better way of doing this than extra states that prevents it being sent multiple times
                if (msg)
                {
                    if (msg->header.msgType != NetMsgType::NETMSG_SERVER_HELLO)
                    {
                        Logging_LogChannel("Client::ConnectOnResolveDone - Server sent back non-server hello!", LogChannel::Warning);
                        return;
                    }

                    Logging_LogChannel("Client received hello", LogChannel::Debug);

                    // change state

                    NetHelloStatus result = static_cast<NetHelloStatus>(msg->Read<uint8_t>());
                    bool success = (result == NetHelloStatus::HELLO_OK);

                    if (success)
                    {
                        Logging_LogChannel("Client hello accepted", LogChannel::Debug);
                        connectPhase = ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD;
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

                        Logging_LogChannel("Connection rejected: %s", LogChannel::Error, errMsg);
                        SetState(ClientState::CLIENT_UNCONNECTED);
                    }
                }
                break; 
            case ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD:
                SendMessage(NetFactory_CreateDownloadStartPacket_Client(), serverAddress);
                connectPhase = ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD_SENT;
                break;
            case ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD_SENT:
                break;
            case ClientConnectionPhase::CLIENT_LETS_GO:     // we are done
                SetState(ClientState::CLIENT_CONNECTED);
                break;
        }

    }
}