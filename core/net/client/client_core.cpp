 #include <core/net/client/client.hpp>

namespace Capy
{
    void Client::Init()
    {
        Logging_LogChannel("Initialising client...", LogChannel::Message);
        
        socket = NET_CreateDatagramSocket(NULL, 0);
        state = ClientState::CLIENT_UNCONNECTED;
        
        port = PORT_DEFAULT;
    }

    void Client::Connect(const char* addr)
    {
        SetState(ClientState::CLIENT_RESOLVING_ADDRESS);

        serverAddress = NET_ResolveHostname(addr);

        if (!serverAddress)
        {
            Logging_LogChannel("Client::Connect - Failed to resolve hostname!", LogChannel::Error);
            SetState(ClientState::CLIENT_FATAL);
            return; 
        }

    }

    void Client::ConnectOnResolveDone()
    {
        
    }

    Client::ClientState Client::GetState()
    {
        return state; 
    }

    void Client::SetState(ClientState _state)
    {
        _state = state;
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
                        Logging_LogChannel("Client::Connect - Connecting...", LogChannel::Error);
                        SetState(ClientState::CLIENT_CONNECTING);
                        break;
                    default:
                        break;
                }
                break;
            case CLIENT_CONNECTING:
                ConnectOnResolveDone();
                break; 
            case CLIENT_SHUTTING_DOWN:
            case CLIENT_FATAL: // will be treated like this later
                Shutdown();
                return;
            case CLIENT_DEAD:
                return; // don't do anything at all, don't even run the code after this
        }
    }   

    void Client::Shutdown()
    {
        Logging_LogChannel("Shutting down client...", LogChannel::Message);

        NET_DestroyDatagramSocket(socket);

        SetState(ClientState::CLIENT_DEAD);
    }
}