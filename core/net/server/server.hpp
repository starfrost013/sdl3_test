/* Server headers */

#pragma once
#include <core/net/net.hpp>

namespace Capy
{
    #define MAX_CLIENTS                 32
    #define PORT_DEFAULT                6769    // 6-7 69

    class Server : public NetMode
    {
        // STRUCTURES AND ENUMS

        friend class Client;

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
            port = PORT_DEFAULT;
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
         
        uint16_t port; 

        Client* clients[MAX_CLIENTS] = {0};
        
        void UpdateWhileRunning();
    };
}
