//
// Starfrost Shared Logging System (Version 4.6.1) 
// An easy to use, multi-channel and multi-destination C/C++ logging system
// Copyright © 2023-2026 starfrost
// 
// Version history - everything before 4.3 is retroactive
//
// Oct 2023 (1.0): 				Wrote it
// Jan 2024 (2.0): 				Fixed it...and added log_channel::Debug.
// Aug 2024 (3.0): 				Fixed it even more, and also made it C++.
// Nov 2024 (3.1): 				Fixed bright colours, simplified interface to use a singleton, other minor tweaks.
// Feb 2025 (3.2): 				Correctly namespaced it, use C++ enum names.
// June 8, 2025	(4.0):			Use C++-style declarations, remove Logger_SetOptions and add fatal_function, a function pointer to call if a fatal log message occurs.
// 								Move from 5 strncat calls to one snprintf call
// June 28, 2025 (4.1):			Add superfatals for when memory corruption makes it not safe to continue
// October 22, 2025 (4.2):		Completely use STD for everything and do not depend on any other functions, use C++ includes
// October 25, 2025 (4.3):		NV1Sim: Made a self-contained version not dependent on any other part of the app with the rewritten console stuff from Volt
//								Removed some extraneous memset's (they were already initialised to zero)
//								Allow the user to specify LogSettings::changed boolean to not override log settings during init
//								Specify version
// December 29, 2025 (4.4): 
//								- Various minor optimisations.
//								- Use camelCase (this is C++!)
// December 31, 2025 (4.4):
//								- Remove stdbool include
// January 22, 2026 (4.5): 		
//								- Use camelCase everywhere
//								- Renamed LogSettings::source to LogSettings::destination
//								- Call std::terminate on superfatals instead of abort
//								- Add sign-on message
//								- Add suppressSignonMessage setting to suppress sign-on message (default false)
//								- Print message if the logger wasn't initialised properly. It doesn't get called if the logger failed to initialise
// February 4, 2026 (4.6):
//								- Add C support back (untested!)
//								- Don't use buffer for setting background/foreground colours (inefficient, should be faster)
//								- Remove whitespace
//								- Fix lingering snake_case
//								- Allow the user to specify the namespace using the USER_NAMESPACE define
// February 11, 2026 (4.7):
//								- Fixed sign-on message being written to file
//								- Formalise comments
//								- Only print into one buffer, and more efficient buffer use

#pragma once
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <iostream>

// This should be updated each time a feature gets added to the logging system.
#define STARFROSTLOG_VERSION 	"Starfrost Shared Logging System 4.7 (February 11, 2026)"

#ifdef __cplusplus

#define USER_NAMESPACE			Capy		// the namespace you want to use in your app

namespace USER_NAMESPACE
{
#endif
	// Enumerate console colours
#ifdef __cplusplus
	enum ConsoleColor
#else
	typedef enum console_color_e
#endif
	{
		Black = 0,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Magenta = 5,
		Cyan = 6,
		White = 7,

		// this is just used to indicate the first "bright" console colour so other code is easier to read
		FirstBright = 8,

		BrightBlack = 8,
		BrightRed = 9,
		BrightGreen = 10,
		BrightYellow = 11,
		BrightBlue = 12,
		BrightMagenta = 13,
		BrightCyan = 14,
		BrightWhite = 15,
#ifdef __cplusplus
	};
#else
	} ConsoleColor; 
#endif

	// Enumerates logging channels. This is a set of flags, so the user can enable anything they want.
#ifdef __cplusplus
	enum LogChannel
#else
	typedef enum log_channel_e 
#endif
	{
		// Prints only on debug builds.
		Debug = 1,
		// Prints advisory messages.
		Message = 2,
		// Prints warnings. The user needs to know about them, but they don't impair program operation.
		Warning = 4,
		// Prints errors that impair program operation.
		Error = 8,
		// Prints fatal errors that require the program to exit.
		Fatal = 16,
		// Memory corruption detected. Even calling Common_Shutdown isn't safe. 
		SuperFatal = 32,
#ifdef __cplusplus
	};
#else
	} LogChannel;
#endif

	// Enumerates available log output sources.
#ifdef __cplusplus
	enum LogDestination
#else
	typedef enum log_destination_e
#endif
	{
		// Logs to standardout using vprintf.
		Printf = 1,
		// Logs to a file. (currently "latest.log")
		File = 2,
#ifdef __cplusplus
	};
#else
	} LogDestination;
#endif

	// Struct storing log settings.
#ifdef __cplusplus
	struct LogSettings
#else
	typedef struct log_settings_s
#endif
	{
		const char* fileName;						// File name to log.
		LogChannel channels;						// Bitmask of logging channels.
		LogDestination destination;					// Bitmask of logging destinations.
		bool keepOldLogs;							// Keep old logs?
		bool changed; 								// Did the log settings change? (only used on Logger_Init so it doesn't overwrite the user's settings)
		bool suppressSignOnMessage;					// Print the sign-on message?
		void (*fatalFunction)();					// Function callback, called on fatal error (optional)
#ifdef __cplusplus
	};
#else
	} LogSettings;
#endif

	// Struct storing the actual logger itself.
#ifdef __cplusplus
	struct Logger
#else
	typedef struct logger_s
#endif
	{
		LogSettings settings;						// Logger settings.
		FILE* handle;								// File handle for logging
		bool initialised;							// Is the logger initialised?
		bool initFailed;							// Did the logger try to initialise, but fail?
#ifdef __cplusplus
	};
#else
	} Logger;
#endif 

	bool Logging_Init();
	void Logging_LogChannel(const char* text, LogChannel channel, ...);
	void Logging_LogAll(const char* text, ...);
	void Logging_Shutdown();

	// Logger object
	extern Logger logger;
#ifdef __cplusplus
}
#endif

