#include <core/game.hpp>
#include <core/render/render.hpp>
#include <data/entities/entity_world.hpp>
#include <net/client/client.hpp>
namespace Capy
{    
    WorldEntity world;                          // TEMP
    Cvar* playerName;

    void Client::Init()
    {
        Logging_LogChannel("Initialising client...", LogChannel::Message);
        
        if (!Render_Init())
        {
            // error will already be printed
            Game_Shutdown();
            return;
        }

        socket = NET_CreateDatagramSocket(NULL, 0);
        state = ClientState::CLIENT_UNCONNECTED;
        
        Cvar_Set("playerName", "Player", false);
        
        port = netPort->value;
    }

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
        SendMessage(NetFactory_CreateDisconnectPacket(NetCastType::NET_CAST_TO_SERVER), serverAddress);
    }

    void Client::ConnectOnResolveDone(NetMsg* msg)
    {
        /* temp */
        world.GetHeader().SetSize(Vector2(3000, 400));
        world.Create();
        
        switch (connectPhase)
        {
            case ClientConnectionPhase::CLIENT_HELLO:
                Logging_LogChannel("Client sending hello", LogChannel::Debug);
                SendMessage(NetFactory_CreateClientHelloPacket(NetCastType::NET_CAST_TO_SERVER), serverAddress);
                break;
            case ClientConnectionPhase::CLIENT_HELLO_SENT: // TODO: figure out a better way of doing this than extra states
                Logging_LogChannel("Client received hello", LogChannel::Debug);
                if (msg)
                {
                    if (msg->header.msgType != NetMsgType::NETMSG_SERVER_HELLO)
                    {
                        Logging_LogChannel("Client::ConnectOnResolveDone - Server sent back non-server hello!", LogChannel::Warning);
                        return;
                    }

                    // change state

                    NetHelloStatus result = static_cast<NetHelloStatus>(msg->Read<uint8_t>());

                    bool success = (result == NetHelloStatus::HELLO_OK);

                    if (success)
                        connectPhase = ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD;
                    else
                    {
                        const char* errMsg = "TODO: Duplicated username handling\n";

                        if (result == NetHelloStatus::HELLO_DUPLICATE_CLIENT)
                            errMsg = "This device is already connected!";
                        else if (result == NetHelloStatus::HELLO_GO_AWAY)
                            errMsg = "Piss Off";

                        Logging_LogChannel(errMsg, LogChannel::Error);
                        SetState(ClientState::CLIENT_UNCONNECTED);
                    }

                }
                break; 
            case ClientConnectionPhase::CLIENT_DOWNLOAD_WORLD:
                
                
                break;
            
        }

        SetState(ClientState::CLIENT_CONNECTED);
    }

    Client::ClientState Client::GetState()
    {
        return state; 
    }

    void Client::SetState(ClientState _state)
    {
        state = _state;
    }

    // Run while the client is connected
    void Client::TickNetwork()
    {
        NetMsg* msg = GetMessage();    

        bool dontCare = false; 

        // if there's no message simply send it through to be checked for
        // otherwise, check the message really comes from the server
        if (msg)
        {
            // 0 = equal, anything else is less
            bool dontCare = NET_CompareAddresses(msg->addr, serverAddress);
            NET_UnrefAddress(msg->addr); //todo: move this to netcore layer
        
        }

        if (dontCare)
            return;

        switch (state)
        {
            case CLIENT_CONNECTING:
                ConnectOnResolveDone(msg);
                break; 
        }   
    }

    void Client::Tick()
    {
        switch (state)
        {
            case CLIENT_UNCONNECTED:
                break;
            case CLIENT_RESOLVING_ADDRESS:
                switch (NET_GetAddressStatus(serverAddress))
                {
                    case NET_FAILURE:
                        Logging_LogChannel("Client::Connect - Failed to resolve hostname!", LogChannel::Error);
                        SetState(ClientState::CLIENT_FATAL);
                        break; 
                    case NET_SUCCESS:
                        Logging_LogChannel("Client::Connect - Connecting...", LogChannel::Message);
                        SetState(ClientState::CLIENT_CONNECTING);
                        connectPhase = ClientConnectionPhase::CLIENT_HELLO;
                        break;
                    default:
                        break;
                }
                break;
            case CLIENT_CONNECTING:
            case CLIENT_CONNECTED:
                TickNetwork();          // usual update function
                break;
            case CLIENT_SHUTTING_DOWN:
            case CLIENT_FATAL:          // will be treated differently later
                Shutdown();
                return;
            case CLIENT_DEAD:
                return;                 // don't do anything at all, don't even run the code after this
        }
    }   

    void Client::Frame()
    {
        //get all incoming messages
        NetMode::Frame();

        switch (state)
        {
            case CLIENT_CONNECTED:
                Render_Clear();
                
                world.Render();
                Render_Present();
                break;
        }
    }

    void Client::Shutdown()
    {
        if (state == CLIENT_CONNECTING 
        || state == CLIENT_CONNECTED)
        {
            Disconnect();
        }
        
        Render_Shutdown();

        Logging_LogChannel("Shutting down client...", LogChannel::Message);
        NET_DestroyDatagramSocket(socket);
        Render_Shutdown();

        SetState(ClientState::CLIENT_DEAD);
    }
}