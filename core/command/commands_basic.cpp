#include "command.hpp"
#include <core/engine.hpp>
#include <core/command/command.hpp>

//
// commands_basic.cpp
// Implements some basic/core commands for the command system
//

namespace Capy
{
    /* Functions only used in this translation unit */
    void Command_CvarCreate(CommandType origin);
    void Command_CvarSet(CommandType origin);
#ifdef DEBUG
    void Command_Echo(CommandType origin);
#endif
    void Command_CvarList(CommandType origin);
    void Command_Shutdown(CommandType origin);

    void Command_CreateBasicCommands()
    {
        // create some basic commands
        Command_Add("create", CommandType::COMMAND_GLOBAL, Command_CvarCreate);
        Command_Add("set", CommandType::COMMAND_GLOBAL, Command_CvarSet);
        Command_Add("exec", CommandType::COMMAND_GLOBAL, Command_Exec); // should this be limited in some way?

        // Debug commands
#ifdef DEBUG
        Command_Add("echo", CommandType::COMMAND_GLOBAL, Command_Echo);
#endif

        // Memory commands
        Command_Add("memstats", CommandType::COMMAND_GLOBAL, Command_MemStats);
        Command_Add("meminfo", CommandType::COMMAND_GLOBAL, Command_MemStats); // alias
        Command_Add("cvarlist", CommandType::COMMAND_GLOBAL, Command_CvarList); // alias
        Command_Add("shutdown", CommandType::COMMAND_GLOBAL, Command_Shutdown); // alias
    }

    void Command_CvarCreate(CommandType origin)
    {
        auto flags = 0;

        if (Command_Argc() < 2)
        {
            Logging_LogChannel("Usage: create <name> <value> [flags]\nCreates a convar.", LogChannel::Message);
            return;
        }
        else if (Command_Argc() >= 3) // if flags were provided
            flags = atoi(Command_Argv(3));

        // remove any "internal" flags if it is a console command
        if (origin == CommandType::COMMAND_CONSOLE)
            flags &= ~(Cvar_NoUser);

        Cvar_Get(Command_Argv(1), Command_Argv(2), (CvarFlags)flags);
    }

    void Command_CvarSet(CommandType origin)
    {
        auto flags = 0;

        if (Command_Argc() < 2)
        {
            Logging_LogChannel("Usage: set <name> <value> [flags]\nSets the value of a convar.", LogChannel::Message);
            return;
        }
        else if (Command_Argc() >= 3) // if flags were provided
            flags = atoi(Command_Argv(3));

        // remove any "internal" flags if it is a console command
        if (origin == CommandType::COMMAND_CONSOLE)
        {
            flags &= ~(Cvar_NoUser);
            Cvar_Set(Command_Argv(1), Command_Argv(2), (CvarFlags)flags, false);
        }
        else
            Cvar_Set(Command_Argv(1), Command_Argv(2), (CvarFlags)flags, true);
    }

#ifdef DEBUG
    void Command_Echo(CommandType origin)
    {
        Logging_LogAll(Command_AllTextAfterName());
    }
#endif

    void Command_CvarList(CommandType origin)
    {
        Cvar_List();
    }

    void Command_Shutdown(CommandType origin)
    {
        Engine_Shutdown();
    }
}