/* Client headers */

#include <core/net/net.hpp>

namespace Capy 
{
    #define CLIENT_NAME_LENGTH              64

    class Client : NetMode
    {
        enum ClientState
        {
            CLIENT_INITIALISING = 0,
            CLIENT_CONNECTING = 1,
            CLIENT_CONNECTED = 2,
            CLIENT_RUNNING = 3,
            CLIENT_DISCONNECTING = 4,
            CLIENT_DISCONNECTED = 5,
            CLIENT_SHUTDOWN = 6,
        };

    public: 
    
        void Init() override; 
        void Shutdown() override; 
        
        ClientState state;
        
        // If the client exists on the server (so we can use faster static allocation)
        bool exists;

    private:
        int id;                         // client unique id
        char name[CLIENT_NAME_LENGTH];  // client name


    };
}
