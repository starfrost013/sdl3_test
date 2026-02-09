/* Server headers */

#include <core/net/net.hpp>
#include <core/net/client/client.hpp>

namespace Capy
{
    #define MAX_CLIENTS                 32

    class Server : NetMode
    {
        // STRUCTURES AND ENUMS

    public: 
        enum ServerState
        {
            SERVER_INITIALISING = 0,
            SERVER_RUNNING = 1,         // Server is available for connections
            SERVER_SHUTTING_DOWN = 2,
        };
    private: 
        ServerState state; 
         
        Client* clients[MAX_CLIENTS];

    public:
        Server(uint16_t _port)
        {
            port = _port;
        }
    
        // METHODS

        void Init() override; 
        void Shutdown() override; 
        
        ServerState GetState();
        void SetState(ServerState state);

        
    };
}
