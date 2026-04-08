/* Entry point and main function for Capybara Wars */

#include "util/logging.hpp"
#include <Capy.hpp>

#include <net/net.hpp>
#include <net/client/client.hpp>
#include <net/server/server.hpp>
#include <core/engine.hpp>

#include <iostream>

namespace Capy
{
    void Capy_Main(int32_t argc, char** argv)
    {
        Engine_Init(argc, argv);

        while (engine.running)
            Engine_Run();

        Engine_Shutdown();
    }
}

int32_t main(int32_t argc, char** argv)
{
    Capy::Capy_Main(argc, argv);
}