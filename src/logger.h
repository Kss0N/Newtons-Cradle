#pragma once

#include <fstream>
#include <iostream>
#include <string>

class logger
{
	static std::wfstream currentLogFile;
public:
	/// @brief Will create a new output file. 
	/// Will name it to the Day/Hour/Minute/Index of the time it was created
	/// Then it will put a JSON-format of the current state of the cradle at the moment of creation 
	static void newLog();

	/// @brief adds a log entry
	static void addLogEntry(std::string);

	/// @brief adds a log entry of type double (used to log out the timesstamps)
	static void addLogEntry(double);

	static inline bool logIsOpen() 
	{ return currentLogFile.is_open(); };

	/// @brief closes the log finally
	inline static void closeLog() { 
		if(currentLogFile.is_open()) 
			currentLogFile.close(); 
	}
};

