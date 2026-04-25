#include <Capy.hpp>
#include <core/script/script.hpp>

namespace Capy
{
    // defines
    #define MINIMUM_REASONABLE_SQSTACK_SIZE     128
    #define DEFAULT_SQSTACK_SIZE                1024

    // convars
    Cvar* scriptStackSize;

    // determines if the scripting system is initialised
    ScriptVM script = {0};

    void Script_Init()
    {
        scriptStackSize = Cvar_Get("scriptStackSize", "1024", false);

        if (scriptStackSize->value < MINIMUM_REASONABLE_SQSTACK_SIZE)
        {
            Logging_LogChannel("Script_Init: Squirrel stack size %d was below the reasonable minimum of %d, setting to default of %d", LogChannel::Warning, 
            scriptStackSize->value, MINIMUM_REASONABLE_SQSTACK_SIZE, DEFAULT_SQSTACK_SIZE);
        }

        Logging_LogChannel("Script_Init: Initialising Squirrel...", LogChannel::Debug);

        script.handle = sq_open(int(scriptStackSize->value));

        if (!script.handle)
            Logging_LogChannel("Failed to initialise Squirrel!", LogChannel::Fatal);

        Logging_LogChannel("Script_Init: Scripting initialised (VM Handle = 0x%lX, stack size = %d)", LogChannel::Message, 
            script.handle, int(scriptStackSize->value));

        sq_setcompilererrorhandler(script.handle, Script_CompileFatal);
        
        script.sqratTable = Sqrat::Table(script.handle);
        script.initialised = true; 
    }

    // send a character to the Squirrel VM
    SQInteger Script_ReadCharacter(SQUserPointer file)           
    {
        FilesystemFile* f = (FilesystemFile*)file;

        if (f->stream->eof())
            return 0x00;
        else
            return f->stream->get();
    }    

    // open and compile a script
    void Script_Open(const char* filename)
    {
        Logging_LogChannel("Script_Open: %s", LogChannel::Debug, filename);
        FilesystemFile* file = Filesystem::Open(filename, FILE_TEXT);

        if (file)
        {
            sq_compile(script.handle, Script_ReadCharacter, file, filename, true);
            Filesystem::Close(file); // todo: make a linked list so we can close stuff that fails e.g. if there is a Script_Fatal call
        }

    }

    void Script_CompileFatal(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source,
                        SQInteger line, SQInteger column)
    {
        Logging_LogChannel("Script compilation failure: %s (BUG!)\nLine %s, Column %s, Script: %s", LogChannel::Fatal,
        desc, line, column, source);
    }
    
    void Script_Shutdown()
    {
        Logging_LogChannel("Script_Shutdown: Shutting down scripting...", LogChannel::Message);

        sq_close(script.handle);
        script.initialised = false; 
        script.handle = nullptr; 
    }
}