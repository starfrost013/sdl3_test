/* Client headers */

#include <core/net/net.hpp>

namespace Capy
{
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
         

    public:
        // METHODS

        void Init() override; 
        void Shutdown() override; 
        
        ServerState GetState();
        void SetState(ServerState state);

        
    };
}
