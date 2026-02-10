/* Client headers */

#pragma once
#include <core/net/net.hpp>

namespace Capy 
{
    #define CLIENT_NAME_LENGTH              64

    class Client : public NetMode
    {
        friend class Server;

    public: 
        enum ClientState
        {
            CLIENT_UNCONNECTED ,
            CLIENT_RESOLVING_ADDRESS,
            CLIENT_CONNECTING,
            CLIENT_CONNECTED,
            CLIENT_RUNNING,
            CLIENT_DISCONNECTING,
            CLIENT_DISCONNECTED,
            CLIENT_SHUTTING_DOWN,
            CLIENT_DEAD,                // Client is dead

            // Fatal error occurred
            CLIENT_FATAL = 0xFF,
        };

        void Init() override; 
        void Connect(const char* addr);
        void Update() override;
        void Shutdown() override; 

        ClientState GetState();
        void SetState(ClientState _state);
        ClientState state;
        
        // If the client exists on the server (so we can use faster static allocation)
        bool exists;


    private:
        int id;                         // client unique id
        char name[CLIENT_NAME_LENGTH];  // client name
        NET_Address* serverAddress;


        void ConnectOnResolveDone();
    };
}
