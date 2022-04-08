#include "cmdParser.h"
#include "Cradle.h"
#include <stdio.h>
#include "CradleCradle.h"
#include "logger.h"




/*
*Commands:
* 
* start			starts timer
* reset			resets values to "compare value" (which means the values of an iron sphere and with g = 9.82. 
* stop			terminates currently running 
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




/// @brief returns the value adjacent to a modifier
/// @param pIterator 
/// @return 0 on fail, value adjacent to modifer on success
/// I really hope I don't need to debug this lmao
_Success_(return != 0) float getVal(const char** pIterator) {
	float val;
	bool readWasSuccessful = false;
	while (**pIterator != '\0' and
		**pIterator != ';' and
		**pIterator != '-' and
		!(**pIterator <= '9' and **pIterator >= '0'))

		*pIterator++;
	if (**pIterator != '\0' and **pIterator != ';')
	{
		if (sscanf_s(*pIterator, "%f", &val) >= 2) readWasSuccessful = true;
	}
	while (**pIterator != '\0' and **pIterator != ';') *pIterator++;
	
	if (readWasSuccessful) return val;
	else return 0;
}

bool isWord(const char* iterator, const char* const word)
{
	int i = 0;
	while (word[i] != '\0' && *iterator != '\0') {
		if (*iterator != word[i++]) return false;
		iterator++;
	}
	return true;
}


void parseCommandLine(const char* command)
{
	auto cmdIterator = command;
	while (*cmdIterator != '\0')
	{
		/*
		* No operands
		*/
		if (isWord(cmdIterator, "START"))
		{
			//start Cradle, dscard anything afterwards
			cradle.simulate();
			break;
		}
		else if (isWord(cmdIterator, "RESET"))
		{
			cradle.reset();
			P::gpScene->setGravity(PxVec3(0, -9.82, 0));
			if (cradle.getSize() >= CradleDefVals::nBalls)
			{
				cradle.removeSegments();
			}
			else
			{
				cradle.addSegments(CradleDefVals::nBalls - cradle.getSize());
			}
		}
		else if (isWord(cmdIterator, "STOP")) {
			cradle.stop();
			break;
		}


		else if (isWord(cmdIterator, "GRAVITY"))
		{
			auto gravity = getVal(&cmdIterator);
			P::gpScene->setGravity(PxVec3(0, -1 * gravity, 0));
			cradle.wakeUp();
			logger::newLog();
		}
		else if (isWord(cmdIterator, "DENSITY"))
		{
			auto density = getVal(&cmdIterator);
			cradle.density = density;
			logger::newLog();
		}
		else if (isWord(cmdIterator, "STATIC"))
		{
			auto staticFriction = getVal(&cmdIterator);
			cradle.staticFriction = staticFriction;
			logger::newLog();
		}
		else if (isWord(cmdIterator, "DYNAMIC"))
		{
			auto dynamicFriction = getVal(&cmdIterator);
			cradle.dynamicFriction = dynamicFriction;
			logger::newLog();
		}
		else if (isWord(cmdIterator, "REST"))
		{
			auto restitution = getVal(&cmdIterator);
			cradle.restitution = restitution;
			logger::newLog();
		}
		else if (isWord(cmdIterator, "ANGLE"))
		{
			auto angle = getVal(&cmdIterator);
			cradle.angleBetweenBallAndJoint = angle;
			cradle.adjustHeightOfJoints();
			logger::newLog();
		}
		else if (isWord(cmdIterator, "HEIGHT"))
		{
			auto height = getVal(&cmdIterator);
			cradle.jointHeight = height;
			cradle.adjustHeightOfJoints();
			logger::newLog();
		}
		else if (isWord(cmdIterator, "BALLS"))
		{
			auto currentSize = cradle.getSize();
			int val = static_cast<int>(getVal(&cmdIterator));
			if (val > currentSize) cradle.addSegments(val-currentSize);
			else cradle.removeSegments(currentSize-val);
			logger::newLog();
		}

		while (*cmdIterator != '\0' and *cmdIterator != ';') cmdIterator++;
	}

}