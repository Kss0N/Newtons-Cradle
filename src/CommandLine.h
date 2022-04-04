#pragma once
#include "framework.h"
#include "CradleCradle.h"
#include <string>
#include <vector>





class CommandLine
{
public:
	
	std::string commandLine = "Hello World";
	std::vector<std::string> previousInputs;


	HWND hCmdWind; //Command Promt Window
	HACCEL hCmdAccel;

	CommandLine();

	void init(HWND = ghMainWindow, HINSTANCE = ghInstance);
};

//CommandLine Singleton
extern CommandLine cmd;