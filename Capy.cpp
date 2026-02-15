/* Entry point and main function for Capybara Wars */

#include "util/logging.hpp"
#include <Capy.hpp>

#include <net/net.hpp>
#include <net/client/client.hpp>
#include <net/server/server.hpp>
#include <core/game.hpp>

#include <iostream>

namespace Capy
{
    void Capy_Main(int32_t argc, char** argv)
    {
        Game_Init(argc, argv);

        while (game.running)
        {
            Game_Run();
        }

        Game_Shutdown();
    }
}

int32_t main(int32_t argc, char** argv)
{
    Capy::Capy_Main(argc, argv);
}