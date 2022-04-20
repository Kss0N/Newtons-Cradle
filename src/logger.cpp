#include "logger.h"
#include "framework.h"
#include <sstream>
#include "CradleCradle.h"
#include "Cradle.h"


std::wfstream logger::currentLogFile;

#define ENDOFLINE "\r\n"


void logger::addLogEntry(std::string s)
{

}

void logger::addLogEntry(double d)
{
	currentLogFile << d << ENDOFLINE;
#ifdef _DEBUG
	std::wstringstream wss;
	wss << "Time: " << d << ENDOFLINE;
	MessageBox(ghMainWindow, wss.str().c_str(), TEXT("PROCESS IS FINISHED"), MB_OK);
#endif // _DEBUG

	

}

void logger::newLog()
{
	if (currentLogFile.is_open()) currentLogFile.close();

	SYSTEMTIME st;
	GetSystemTime(&st);

	std::wstringstream ss;
	ss 
		<< "LogEntry "
		<< st.wDay << '/' 
		<< st.wHour << '/'
		<< st.wMinute;

	currentLogFile.open(ss.str(), std::ios::app);
	if (!currentLogFile.is_open()) throw errno;
	//Add Date
	currentLogFile << '['
		<< st.wDay << " | "
		<< st.wHour << " | "
		<< st.wMinute << " | "
		<< ']' << ENDOFLINE;

	/*Log State*/
	currentLogFile << '{' << ENDOFLINE
		<< '\t' << "gravity: " << -P::gpScene->getGravity().y << ENDOFLINE
		<< '\t' << "density: " << cradle.density << ENDOFLINE
		<< '\t' << "radius :" << cradle.ballRadius << ENDOFLINE
		<< '\t' << "staticFriction :" << cradle.staticFriction << ENDOFLINE
		<< '\t' << "dynamicFriction :" << cradle.dynamicFriction << ENDOFLINE
		<< '\t' << "restitution :" << cradle.restitution << ENDOFLINE
		<< '\t' << "angle :" << cradle.angleBetweenBallAndJoint << ENDOFLINE
		<< '\t' << "height :" << cradle.jointHeight << ENDOFLINE
		<< '\t' << "nBalls :" << cradle.getSize() << ENDOFLINE
		<< '}' << ENDOFLINE;
}
