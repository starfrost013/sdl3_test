#include <core/command/command.hpp>

//
// command.cpp
// Implements the command system
// 

namespace Capy
{
    // Globals
    Command* commandHead = nullptr;
    Command* commandTail = nullptr;

    /* This is probably not threadsafe */
    char lastCommand[STRING_MAX] = {0};
    /* This is so we can do stuff like strtok */
    char lastToken[STRING_MAX] = {0};

    void Command_Init()
    {
        Logging_LogChannel("******** Command_Init ********", LogChannel::Debug);

        Command_CreateBasicCommands();
    }

    /* Adds a command. */
    Command* Command_Add(const char* name, CommandType type, void (*on_execute)(CommandType origin))
    {
        if (!name || strlen(name) <= 0)
        {
            Logging_LogChannel("Tried to add an invalid command name", LogChannel::Error);
            return nullptr; 
        }

        Command* command = new Command();

        // if there are no commands, add some
        if (!commandHead)
            commandHead = commandTail = command;
        else
        {
            // do we need this list to be doubly linked?
            command->prev = commandTail;
            commandTail->next = command;
            commandTail = command;
        }

        command->name = name;
        command->type = type;
        command->onExecute = on_execute;

        Logging_LogChannel("Creating command %s", LogChannel::Debug, command->name);

        return command;
    }

    // Finds a command by its name
    Command* Command_FindByName(const char* name)
    {
        if (!name)
            return nullptr;

        Command* current_command = commandHead;

        while (current_command)
        {
            if (!strcasecmp(current_command->name, name))
                return current_command;
                
            current_command = current_command->next;
        }

        return nullptr;
    }

    uint32_t Command_Argc()
    {
        // restore the original
        strncpy(lastToken, lastCommand, STRING_MAX);

        char* cmd_name = strtok(lastToken, STRING_WHITESPACE_DELIMITERS);

        uint32_t amount = 0;

        // skip the command name

        char* tok = strtok(NULL, STRING_WHITESPACE_DELIMITERS);

        while (tok)
        {
            amount++;
            tok = strtok(NULL, STRING_WHITESPACE_DELIMITERS);
        }

        return amount; 
    }

    // Return all the text after the command name for the last command executed
    const char* Command_AllTextAfterName()
    {
        // restore the original
        strncpy(lastToken, lastCommand, STRING_MAX);
        char* cmd_name = strtok(lastToken, STRING_WHITESPACE_DELIMITERS);
        // skip the command name

        uint32_t name_length = strlen(cmd_name);

        if (name_length >= STRING_MAX)
            return lastToken; 

        char* tok = lastToken + name_length + 1;
        return tok; 
    }

    // Return parameter "argv" of command "command".
    const char* Command_Argv(uint32_t argv)
    {
        // restore the original
        strncpy(lastToken, lastCommand, STRING_MAX);

        char* str = Util_GetTokenSeparatedPart(lastToken, " ", argv);
        
        uint32_t position = strlen(str);

        while (position > 0)
        {
            uint8_t last_char = str[position];

            if (isspace(last_char) 
            || str[position] == '\n'
            || str[position] == '\r')
            {
                str[position] = '\0'; //terminate the string early
            }
            
            position--;
        }

        if (!str)
            return STRING_EMPTY;
    
        return str; 
    }

    void Command_Execute(const char* cmd, CommandType origin)
    {
        auto len = strlen(cmd);

        if (len > STRING_MAX)
            Logging_LogChannel("Command_Execute: Command string above %d characters. The command will be executed, but you won't be able to access parameters after the cutoff",
            LogChannel::Warning, STRING_MAX);

        strncpy(lastCommand, cmd, STRING_MAX);
        strncpy(lastToken, cmd, STRING_MAX);

        char* cmdName = strtok(lastToken, STRING_WHITESPACE_DELIMITERS);

        // silently return if no command is entered at all
        if (!cmdName
        || !strlen(cmdName))
        {
            return; 
        }

        Command* commandPtr = Command_FindByName(cmdName);

        if (!commandPtr)
        {
            Logging_LogChannel("Command not found: %s", LogChannel::Warning, cmdName);
            return;
        }

        if (!commandPtr->onExecute)
        {
            Logging_LogChannel("Command has no on-execute function: %s", LogChannel::Error, cmdName);
            return;
        }

        if (commandPtr->type & origin)
            commandPtr->onExecute(origin);
    }

    /* Executes global type commands*/
    void Command_ExecuteGlobal(const char* cmd)
    {
        Command_Execute(cmd, CommandType::COMMAND_GLOBAL);
    }

    /* Shuts down the command system*/
    void Command_Shutdown()
    {
        // nothing to do
        if (!commandHead || !commandTail)
            return; 

        // free everything in the list.
        Command* command = commandHead;

        while (command)
        {
            Command* next_free = command->next;
            delete command; 
            command = next_free;
        }

        commandHead = commandTail = NULL;
    }
}