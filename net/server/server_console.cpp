#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    // use raw data obtained from getline
    // ***** THE OTHER THREAD ONLY EVER READS FROM THIS SO IT IS SAFE *****
    // NEVER EVER EVER WRITE CODE LIKE THIS!
    std::string consoleBuf;

    // Get console input asynchronously.
    char* Server::ConsoleInputThread()
    {
        std::getline(std::cin, consoleBuf);
        return consoleBuf.data();
    }
};