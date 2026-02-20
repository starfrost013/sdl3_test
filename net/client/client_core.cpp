//
// client_core.cpp: Client init, loop, network update & shutdown code
//

#include <core/game.hpp>
#include <core/render/render.hpp>
#include <data/entities/entity_world.hpp>
#include <net/client/client.hpp>

namespace Capy
{    
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
    }

    // Get the client state
    Client::ClientState Client::GetState()
    {
        return state; 
    }

    // Set the client state
    void Client::SetState(ClientState _state)
    {
        state = _state;
    }

    // Send a message to the server
    void Client::SendMessage(NetMsg msg, NET_Address* address)
    {
        SendMessageToPort(msg, address, netPort->value);
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

        //delete the message
        if (msg)
            DoneMessage(msg);
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
                        connectPhase = ConnectPhase::CLIENT_HELLO;
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

        // Test code
        world.Serialise("Test.world");
        
        Render_Shutdown();

        Logging_LogChannel("Shutting down client...", LogChannel::Message);
        NET_DestroyDatagramSocket(socket);
        Render_Shutdown();

        SetState(ClientState::CLIENT_DEAD);
    }
}