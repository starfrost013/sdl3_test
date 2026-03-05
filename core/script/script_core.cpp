#include <core/script/script.hpp>

namespace Capy
{
    // defines
    #define MINIMUM_REASONABLE_SQSTACK_SIZE     128
    #define DEFAULT_SQSTACK_SIZE                1024

    // convars
    Cvar* scriptStackSize;

    // determines if the scripting system is initialised
    ScriptVMState scriptVm = {0};

    void Script_Init()
    {
        scriptStackSize = Cvar_Get("scriptStackSize", "1024", false);

        if (scriptStackSize->value < MINIMUM_REASONABLE_SQSTACK_SIZE)
        {
            Logging_LogChannel("Script_Init: Squirrel stack size %d was below the reasonable minimum of %d, setting to default of %d", LogChannel::Warning, 
            scriptStackSize->value, MINIMUM_REASONABLE_SQSTACK_SIZE, DEFAULT_SQSTACK_SIZE);
        }

        Logging_LogChannel("Script_Init: Initialising Squirrel...", LogChannel::Debug);

        scriptVm.handle = sq_open(int(scriptStackSize->value));

        if (!scriptVm.handle)
            Logging_LogChannel("Failed to initialise Squirrel!", LogChannel::Fatal);

        Logging_LogChannel("Script_Init: Scripting initialised (VM Handle = 0x%lX, stack size = %d)", LogChannel::Message, 
            scriptVm.handle, int(scriptStackSize->value));

        scriptVm.initialised = true; 
    }

    void Script_Fatal()
    {

    }
    
    void Script_Shutdown()
    {
        sq_close(scriptVm.handle);
        scriptVm.initialised = false; 
        scriptVm.handle = nullptr; 

        Logging_LogChannel("Script_Shutdown: Shut down scripting", LogChannel::Message);
    }
}