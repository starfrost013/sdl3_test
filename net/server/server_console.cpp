#include <net/client/client.hpp>
#include <net/server/server.hpp>

namespace Capy
{
    // use raw data obtained from getline
    // ***** THE OTHER THREAD ONLY EVER READS FROM THIS SO IT IS SAFE *****
    // NEVER EVER EVER WRITE CODE LIKE THIS!
    std::string consoleBuf;

    void Server::ConsoleUpdate()
    {
        // maybe a bad idea
        if (consoleInputProc.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            Command_Execute(consoleInputProc.get(), CommandType::COMMAND_CONSOLE);

            // restart
            consoleInputProc = std::async(std::launch::async, &Server::ConsoleInputThread);
        }
            
    }

    // Get console input asynchronously.
    char* Server::ConsoleInputThread()
    {
        std::getline(std::cin, consoleBuf);
        return consoleBuf.data();
    }
};