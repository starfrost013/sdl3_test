/* 
    Capybara Wars Project
    A game for Windows & Linux - Copyright © 2025, 2026 starfrost
    Script.hpp: squirrel bridge 
    not a static class because of the fact that squirrel is C-oriented
    
*/

#pragma once 

// includes
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdaux.h>
#include <sqstdimport.h>
#include <sqstdmodule.h>
#include <sqrat.h>

class Cvar;

namespace Capy
{
    class ScriptVM
    {
    public: 
        bool initialised;
        HSQUIRRELVM handle;
        Sqrat::Table sqratTable;
        
    };

    // determines if the scripting system is initialised
    extern ScriptVM script;

    void Script_Init();                                                         // initialise the squirrel bridge
    SQInteger Script_ReadCharacter(SQUserPointer file);                         // send a character to the Squirrel VM
    void Script_Open(const char* filename);                                     // open a script
    void Script_CompileFatal(HSQUIRRELVM vm, const SQChar* ch, const SQChar* source,
                      SQInteger line, SQInteger column);                        // fatal error reported by Squirrel VM
    
    template <typename T> Sqrat::Class<T> Script_ExposeClass(const char* name)
    {
        Sqrat::Class<T> theClass(script.handle, name);
        return theClass; 
    }

    template <typename T, class F> void Script_ExposeMethod(const char* name, Sqrat::Class<T> sqratClass, F method)
    {
        sqratClass.Func(name, method);
    }

    template <typename T, class V> void Script_ExposeVar(const char* name, Sqrat::Class<T> sqratClass, V var)
    {
        sqratClass.Var(name, var);
    }

    // call after above
    template <typename T> void Script_ClassDone(const char* name, Sqrat::Class<T> sqratClass)
    {
        script.sqratTable.Bind(name, sqratClass);
    }

    void Script_Shutdown();                                                     // SHutdown
}
