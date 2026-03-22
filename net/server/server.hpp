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
        void Frame() override; 
        void Shutdown() override; 
        
        ServerState GetState();
        void SetState(ServerState _state);

    private: 
        ServerState state; 

        uint16_t port;

        // console input
        void ConsoleUpdate();
        static char* ConsoleInputThread();
        std::future<char*> consoleInputProc;

        // send message
        void SendMessage(NetMsg msg, Client* client);                       // Send a message to a client.
        void SendMessageToAll(NetMsg msg);                                  // Send a message to all clients.
        void SendMessageToAll(NetMsg msg, Client* exclude);                 // Send a message to all clients except the "exclude" client

        // intenral tick methods
        void TickNetwork();
        void TickNetwork_ClientMessage(Client* client, NetMsg* msg);        // Client message was received
        void TickDownloads();                                               // Update world downloads
    
        // client related methods
        bool ClientIsNew(NetMsg* msg, const char* username);
        void ClientNew(NetMsg* hello);
        void ClientRemove(Client* client);
        void ClientStartWorldDownload(Client* client);
        void ClientSendWorldChunk(Client* client);

        
        // Client search functions
        Client* ClientByIp(NetMsg* msg);                                    // get client by ip given netmsg
        Client* ClientByIpPort(NetMsg* msg);                                // get client by (ip, port) given netmsg
        Client* ClientByUsername(const char* username);                     // get client by username
        
        void SetMap();

    };
}
