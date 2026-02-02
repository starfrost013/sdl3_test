/* Client headers */

#include <core/net/net.hpp>

namespace Capy
{
    class Server 
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
         

    public:
        // METHODS

        ServerState GetState();
        void SetState(ServerState state);

        
    };
}
