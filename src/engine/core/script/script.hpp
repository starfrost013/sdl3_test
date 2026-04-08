/* 
    Capybara Wars Project
    A game for Windows & Linux - Copyright © 2025, 2026 starfrost
    Script.hpp: squirrel bridge 
    not a static class because of the fact that squirrel is C-oriented
    
*/

#pragma once 

// includes
#include <Capy.hpp>
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdaux.h>
#include <sqstdimport.h>
#include <sqstdmodule.h>

namespace Capy
{
    // convars
    extern Cvar* scriptStackSize;

    struct ScriptVMState
    {
        bool initialised;
        HSQUIRRELVM handle;
    };

    // determines if the scripting system is initialised
    extern ScriptVMState script;

    void Script_Init();                                                         // initialise the squirrel bridge
    SQInteger Script_ReadCharacter(SQUserPointer file);                         // send a character to the Squirrel VM
    void Script_Open(const char* filename);                                     // open a script
    void Script_CompileFatal(HSQUIRRELVM vm, const SQChar* ch, const SQChar* source,
                      SQInteger line, SQInteger column);                        // fatal error reported by Squirrel VM
    void Script_Shutdown();                                                     // SHutdown
}
