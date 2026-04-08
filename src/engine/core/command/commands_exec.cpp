#include <core/command/command.hpp>

//
// commands_exec.cpp
// Implements the "exec" command. 
//

namespace Capy
{
    FilesystemFile* execCfg = nullptr;
    
    void Command_Exec(CommandType origin)
    {       
        if (Command_Argc() < 1)
        {
            Logging_LogChannel("Usage: exec <path to cfg file>", LogChannel::Message);
            return;
        }

        const char* file_path = Command_Argv(1);

        execCfg = Filesystem::Open(file_path, FilesystemFileMode::FILE_TEXT);

        if (!execCfg)
        {
            Logging_LogChannel("exec: File not found!", LogChannel::Error);
            return;
        }

        char ch = 0x01;
        char execBuf[STRING_MAX] = {0}; 

        while (!execCfg->stream.eof())
        {
            auto pos = execCfg->stream.tellp();
            // yes this reads some irrelevant data but i don't want to use std::string
            execCfg->stream.getline(execBuf, STRING_MAX); 
            auto length = strlen(execBuf);
            execCfg->stream.seekp(pos + length, std::_S_beg);
        
            Command_Execute(execBuf, CommandType::COMMAND_CONSOLE);

        }

    }
}
