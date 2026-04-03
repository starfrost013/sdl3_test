/* Client headers */

#pragma once
#include <net/net.hpp>


namespace Capy 
{
    class Client; 

    #define CLIENT_NAME_MAX              64
    #define CLIENT_IP_LENGTH                32

    //
    // Temporary Cvars
    //

    extern Cvar* playerName;


    typedef void (Client::*ClientRpcMethod)(NetMsg* msg); 

    class Client : public NetMode
    {
        friend class Server;

    public: 
        enum ClientState
        {
            CLIENT_UNCONNECTED,
            CLIENT_RESOLVING_ADDRESS,
            CLIENT_CONNECTING,
            CLIENT_CONNECTED,
            CLIENT_DISCONNECTING,
            CLIENT_DISCONNECTED,
            CLIENT_SHUTTING_DOWN,
            CLIENT_DEAD,                // Client is dead

            // Fatal error occurred
            CLIENT_FATAL = 0xFF,
        };

        // Enumeration for the client connection phases
        enum ConnectPhase
        {
            // these are logical pairs so put them on the same line
            CLIENT_HELLO, CLIENT_HELLO_SENT,
            CLIENT_DOWNLOAD_WORLD, CLIENT_DOWNLOAD_WORLD_SENT,
            CLIENT_DOWNLOADING_WORLD,
            CLIENT_SPAWN_CHARACTER, CLIENT_SPAWN_CHARACTER_SENT,
            CLIENT_LETS_GO,
        };
        
        void Init() override;                       // Called on init
        void Connect(const char* addr);
        void Disconnect(const char* reason);
        void Tick() override;
        void Frame() override; 
        void Shutdown() override; 

        ClientState GetState();
        void SetState(ClientState _state);

    private:
        int id;                         // client unique id
        char name[CLIENT_NAME_MAX];  // client name
        NET_Address* serverAddress;

        ClientState state;
        // only valid if state == CLIENT_CONNECTING
        ConnectPhase connectPhase;

        // if client is downloading a file, store progress here
        // probably a tmeporary implementation
        size_t downloadProgress; 

        void TickNetworkConnecting(NetMsg* msg);
        void TickNetworkConnected(NetMsg* msg);
        void ConnectDownloadWorldChunk(NetMsg* msg);
        void TickNetwork();

        void OnRpcReceive(NetMsg* msg) override; 
        void OnRpcClientCreate(NetMsg* msg);
        void OnRpcClientDelete(NetMsg* msg);

        void SendMessage(NetMsg msg, NET_Address* address);

        // STUFF BELOW HERE ONLY EXISTS ON THE SERVER'S VERSION OF THE CLIENT AND CANNOT BE DEPENDED ON
        struct ServerOnlyClientInfo
        {
            NET_Address* address; 
            char ipStr[CLIENT_IP_LENGTH];
            uint16_t port; 

        }; 

        ServerOnlyClientInfo serverOnly;

        // RPC method table
        std::unordered_map<uint8_t, ClientRpcMethod> rpcReceiveMethods =
        {
            { NETMSG_RPC_REQUEST_CLIENT_CREATE, &Client::OnRpcClientCreate },
            { NETMSG_RPC_REQUEST_CLIENT_DELETE, &Client::OnRpcClientDelete },
        };
    };
}
