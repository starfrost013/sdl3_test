#include <cmdline/cmdline.hpp>

//
// cmdline.cpp
// Logic for the command line subsystem
// 

namespace Brewery
{
	uint32_t cmdline_argc;					// Holds the number of the engine's command line options
	char** cmdline_argv;					// Holds a pointer to the engine's command line options

	void Cmdline_Init(int32_t argc, char** argv)
	{
		// is this safe on all platforms?
		cmdline_argv = argv;
		cmdline_argc = argc;
	}
	
	// Check if a command line option exists.
	bool Cmdline_Check(const char* option)
	{
		for (uint32_t arg = 0; arg < cmdline_argc; arg++)
		{
			if (!strcasecmp(cmdline_argv[arg], option))
				return true;
		}
	
		return false; 
	}

	uint32_t Cmdline_Argc()
	{
		return cmdline_argc;
	}

	const char* Cmdline_Argv(uint32_t c)
	{
		if (c >= cmdline_argc
			|| c < 0)
		{
			Logging_LogChannel("Cmdline_Argv: Tried to obtain invalid command line option %d (range 0-%d). Returning empty string...", 
				LogChannel::Error, c, cmdline_argc); 
			return STRING_EMPTY;
		}

		return cmdline_argv[c];
	}


	/* See if +exec was provided, so we can create the machine right after init */
	void Cmdline_ParseExec()
	{
		char exec_buf[STRING_MAX] = {0};

		/* -2 because we need to provide a "+set", cvar name, and the value*/
		for (uint32_t arg = 0; arg < cmdline_argc - 1; arg++)
		{
			// don't crash
			if (!cmdline_argv[arg])
				break;
		}
	}
}