/* Client headers */

#include <core/net/net.hpp>

namespace Capy 
{
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

        ClientState state;
    };
}
