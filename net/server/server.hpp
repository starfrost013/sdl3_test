/* Server headers */

#pragma once
#include <net/net.hpp>

namespace Capy
{
    extern Cvar* map; 

    // TEMPORARY CVARS UNTIL WE HAVE A MENU
    extern Cvar* mapSizeX; 
    extern Cvar* mapSizeY; 

    class Server : public NetMode
    {
        // STRUCTURES AND ENUMS

    public: 
        enum ServerState
        {
            SERVER_INITIALISING,
            SERVER_RUNNING,             // Server is available for connections
            SERVER_SHUTTING_DOWN,
            SERVER_DEAD,
        };

        Server()
        {
            port = netPort->value;

            if (!netPort->value)
            {
                Cvar_Set("netPort", "6769", false);
                port = netPort->value;
            }
        }

        Server(uint16_t _port)
        {
            port = _port;
        }
    
        // METHODS

        void Init() override; 
        void Tick() override;
        void Shutdown() override; 
        
        ServerState GetState();
        void SetState(ServerState _state);

    private: 
        ServerState state; 
        Client* clients[MAX_CLIENTS] = {0};
        int32_t numClients; 
        uint16_t port;

        // send message
        void SendMessage(NetMsg msg, Client* client);

        // intenral tick methods
        void TickNetwork();
        void TickNetwork_ConnectedClientMessage(Client* client, NetMsg* msg);

        // client related methods
        bool IsNewClient(NET_Address* address);
        void NewClient(NetMsg* hello);
        void RemoveClient(Client* client);
        void SetMap();
        Client* GetMessageSender(NET_Address* address);
    };
}
