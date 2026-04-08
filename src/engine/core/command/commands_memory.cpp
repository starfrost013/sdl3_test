#include <core/command/command.hpp>

//
// commands_memory.cpp
// Implements the memory commands. 
//

namespace Capy
{
    void Command_MemStats(CommandType origin)
    {
        Logging_LogChannel("%d allocations (total memory use: %d bytes)", LogChannel::Message, Memory_GetNumAllocs(), Memory_GetTotalRam());
    }
}