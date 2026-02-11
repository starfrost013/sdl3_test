// 
// SSLS
// Copyright © 2023-2026 starfrost
//
// Logging.cpp: Handles logging for the Volt project
//

#include "logging.hpp"

#ifdef __cplusplus
namespace USER_NAMESPACE
{
#endif
	// Defines 
	#define	LOGGING_MAX_LENGTH_TEXT			1024												// Maximum length of text being logged via the logger.
	#define LOGGING_MAX_LENGTH_DATE			32													// Length of the string containing the current date.
	#define LOGGING_MAX_LENGTH_TOTAL		LOGGING_MAX_LENGTH_TEXT + LOGGING_MAX_LENGTH_DATE	// Total length of the logging text buffer.

	#define ITS_OVER						0x43524150											// 'CRAP' Exit code used on failure

	// Prototypes for "internal" functions only used in this source file
	void Logging_Log(const char* text, LogChannel channel, va_list args);

	// various buffer size demands
	#define CONSOLE_TERMINAL_COMMAND_PREFIX "\x1B["

	void Util_ConsoleSetForegroundColor(ConsoleColor color)
	{
		int32_t colorFinal = 30 + color;

		if (color >= ConsoleColor::FirstBright) 
			colorFinal = 90 + (color & ConsoleColor::FirstBright - 1);

		printf("%s%dm", CONSOLE_TERMINAL_COMMAND_PREFIX, colorFinal);
	}

	void Util_ConsoleSetBackgroundColor(ConsoleColor color)
	{
		int32_t colorFinal = 40 + color;

		if (color >= ConsoleColor::FirstBright) 
			colorFinal = 100 + (color & ConsoleColor::FirstBright - 1);

		printf("%s%dm", CONSOLE_TERMINAL_COMMAND_PREFIX, colorFinal);
	}

	void Util_ConsoleResetForegroundColor()
	{
		printf(CONSOLE_TERMINAL_COMMAND_PREFIX "39m");
	}

	void Util_ConsoleResetBackgroundColor()
	{
		printf(CONSOLE_TERMINAL_COMMAND_PREFIX "49m");
	}

	// not threadsafe :O
	Logger logger = { 0 };

	// Implementation
	bool Logging_Init()
	{
		//todo: allow this to be configured in settings
		if (!logger.settings.changed)
		{
			logger.settings.channels = (LogChannel)(LogChannel::Debug | LogChannel::Message | LogChannel::Warning | LogChannel::Error | LogChannel::Fatal | LogChannel::SuperFatal);
			logger.settings.destination = (LogDestination)(LogDestination::Printf | LogDestination::File);
			logger.settings.keepOldLogs = false; 
		}

		if (!logger.settings.fileName) 
			logger.settings.fileName = "latest.log";

		if (logger.settings.destination & LogDestination::File)
		{
			logger.handle = fopen(logger.settings.fileName, "w+");

			if (!logger.handle)
			{
				printf("Log failed: 0x0002DEAD Error opening logfile %s: errno %d\n", logger.settings.fileName, errno);
				logger.initFailed = true; 
			}
		}

		if (logger.initFailed)
			return false;

		// we didn't fail so go ahead and set the variables. Any error they will get will be printed above

		logger.initialised = true;
		logger.initFailed = false;

		if (!logger.settings.suppressSignOnMessage)
			Logging_LogChannel(STARFROSTLOG_VERSION " initialised", LogChannel::Debug);

		return true;
	}

	void Logging_Shutdown()
	{
		fclose(logger.handle);
	}

	void Logging_LogChannel(const char* text, LogChannel channel, ...)
	{
		va_list	args;

		va_start(args, channel);

		Logging_Log(text, channel, args);
	}

	// Logs to all channels except the Fatal Error log channel.
	void Logging_LogAll(const char* text, ...)
	{
		va_list args;

		va_start(args, text);

		Logging_Log(text, (LogChannel)(LogChannel::Debug | LogChannel::Message | LogChannel::Warning | LogChannel::Error), args);
	}

	// yes this is required
	void Logging_Log(const char* text, LogChannel channel, va_list args)
	{
		if (!logger.initialised && !logger.initFailed)
		{
			printf("SSLS: Call Logging_Init!\n");
			return; 
		}

		// don't print messages on closed log channels
		if (!(logger.settings.channels & channel))
			return;

	// remove debug channel on release build
	#ifdef RELEASE
		channel &= ~(LogChannel::Debug);
	#endif

		if (strlen(text) > LOGGING_MAX_LENGTH_TEXT)
		{
			printf("Log failed: Maximum log string length is %d bytes!", LOGGING_MAX_LENGTH_TEXT);
			return;
		}

		char dateBuffer[LOGGING_MAX_LENGTH_DATE] = {0};
		char logStringBuffer[LOGGING_MAX_LENGTH_TOTAL] = {0};

	#ifdef __cplusplus
		auto now = std::chrono::system_clock::now();
		std::time_t nowTimeT = std::chrono::system_clock::to_time_t(now);
	#else
		time_t time;
		struct tm* timeInfo;

		time(&time);
		timeInfo = localtime(&time);
		time_t nowTimeT = asctime(timeInfo);
	#endif
		snprintf(dateBuffer, LOGGING_MAX_LENGTH_DATE, "%s", std::ctime(&nowTimeT));

		// lop off the last character so it doesn't have a new line
		size_t dateBufferLength = strlen(dateBuffer);
		dateBuffer[dateBufferLength - 1] = '\0';

		const char* prefix = "[";
		const char* suffixDate = "]: ";
		const char* suffix = " \n";

		// print separate colours
		switch (channel)
		{
			case LogChannel::Message:	//shutup gcc
				break;
			case LogChannel::Debug:
				prefix = "[DEBUG] [";
				break;		
			case LogChannel::Warning:
				prefix = "[WARNING] [";
				break;
			case LogChannel::Error:
				prefix = "[ERROR] [";
				break;
			case LogChannel::Fatal:
				prefix = "[FATAL] [";
				break;
			case LogChannel::SuperFatal:
				prefix = "[SUPER FATAL] [";
				break;
		}

		snprintf(logStringBuffer, sizeof(logStringBuffer), 
		"%s%s%s %s %s", prefix, dateBuffer, suffixDate, text, suffix);

		if (logger.settings.destination & LogDestination::Printf)
		{
			switch (channel)
			{
				case LogChannel::Message: //shutup GCC
					break;
				case LogChannel::Debug:
					Util_ConsoleSetForegroundColor(ConsoleColor::BrightBlue);
					break;
				case LogChannel::Warning:
					Util_ConsoleSetForegroundColor(ConsoleColor::BrightYellow);
					break;
				case LogChannel::Error:
				case LogChannel::Fatal:
				case LogChannel::SuperFatal:
					Util_ConsoleSetForegroundColor(ConsoleColor::BrightRed);
					break;
			}

			vprintf(logStringBuffer, args);

			Util_ConsoleResetForegroundColor();

			// If a fatal error message is logged, we're going down,
			// so call the fatal error function if it is safe to do so
			if (channel & LogChannel::Fatal
			&& !(channel & LogChannel::SuperFatal)
			&& logger.settings.fatalFunction)
			{
				logger.settings.fatalFunction();
			}
		}

		if (logger.settings.destination & LogDestination::File)
			vfprintf(logger.handle, logStringBuffer, args);

		if (channel & LogChannel::SuperFatal)
		{
			printf("MEMORY COMPLETELY HOSED. NOT SAFE TO CLEANLY SHUT DOWN - LET'S GET THE HELL OUT OF HERE!!!\n");
#ifdef __cplusplus
			std::terminate();
#else
			abort();
#endif
		}

		va_end(args);
	}
#ifdef __cplusplus
}
#endif