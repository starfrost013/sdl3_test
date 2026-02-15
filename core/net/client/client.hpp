/* Client headers */

#pragma once
#include <core/net/net.hpp>

namespace Capy 
{
    #define CLIENT_NAME_LENGTH              64

    //
    // Temporary Cvars
    //

    extern Cvar* playerName;

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
        enum ClientConnectionPhase
        {
            // these are logical pairs so put them on the same line
            CLIENT_HELLO, CLIENT_HELLO_SENT,
            CLIENT_DOWNLOAD_WORLD, CLIENT_DOWNLOAD_WORLD_SENT,
            CLIENT_SPAWN_CHARACTER, CLIENT_SPAWN_CHARACTER_SENT,
            CLIENT_LETS_GO,
        };
        
        bool connecting; // connectPhase used above (todo: can be refactored to be the same as the server)

        void Init() override; 
        void Connect(const char* addr);
        void Tick() override;
        void Frame() override; 
        void Shutdown() override; 

        ClientState GetState();
        void SetState(ClientState _state);

        // If the client exists on the server (so we can use faster static allocation)
        bool exists;

    private:
        int id;                         // client unique id
        char name[CLIENT_NAME_LENGTH];  // client name
        NET_Address* serverAddress;

        ClientState state;
        // only valid if state == CLIENT_CONNECTING
        ClientConnectionPhase connectPhase;

        void ConnectOnResolveDone(NetMessage* msg);
        void TickNetwork();

        // STUFF BELOW HERE ONLY EXISTS ON THE SERVER'S VERSION OF THE CLIENT AND CANNOT BE DEPENDED ON
        struct ServerOnlyClientInfo
        {
            NET_Address* address; 
        }; 

        ServerOnlyClientInfo serverOnly;

    };
}
