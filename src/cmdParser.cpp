#include "cmdParser.h"
#include "Cradle.h"
#include <stdio.h>



/*
*Commands:
* 
* start			starts timer
* 
* gravity		g-constant
* density		mass/volume
* static		static friction
* dynamic		dynamic friction
* rest			restitution
* angle			angle between ball and hanger
* height		height from ball to hanger in Y axis
* balls			ammount of Balls
*/



static constexpr auto GRAVITY_COMMAND = 1;
static constexpr auto DENSITY_COMMAND = 2;
static constexpr auto STATIC_COMMAND = 3;
static constexpr auto DYNAMIC_COMMAND = 4;
static constexpr auto REST_COMMAND = 5;
static constexpr auto ANGLE_COMMAND = 6;
static constexpr auto HEIGHT_COMMAND = 7;
static constexpr auto BALLS_COMMAND = 8;


void parseCommand(const char* line, int COMMAND) {
	auto cmdIterator = line;
	float val;
	
}

void parseCommandLine(const char* command)
{
	auto cmdIterator = command;
	while (*cmdIterator != '\0')
	{
		if (sscanf(cmdIterator, "start") >= 2)
		{
			//start Cradle, dscard anything afterwards

			break;
		}
	}
EXIT:

}